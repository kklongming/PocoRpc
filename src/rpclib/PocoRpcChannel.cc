/* 
 * File:   PocoRpcChannel.cc
 * Author: kk
 * 
 * Created on 2013年8月12日, 下午7:33
 */

#include "rpclib/PocoRpcController.h"
#include "rpclib/PocoRpcChannel.h"
#include "rpclib/BytesBuffer.h"
#include "rpclib/PocoRpcError.h"
#include "rpclib/PocoRpcSocketReactor.h"
#include "rpc_def/base_service.pb.h"
#include "rpc_proto/poco_rpc.pb.h"
#include "base/runable.h"
#include "base/base.h"

#include <Poco/Thread.h>
#include <Poco/Mutex.h>
#include <Poco/ScopedLock.h>
#include <Poco/Net/Socket.h>
#include <Poco/Net/StreamSocket.h>
#include <Poco/Net/SocketAddress.h>
#include <Poco/Net/SocketNotification.h>
#include <Poco/Net/SocketReactor.h>
#include <Poco/Timespan.h>
#include <Poco/Exception.h>
#include <Poco/NObserver.h>
#include <Poco/UUID.h>
#include <Poco/UUIDGenerator.h>

DEFINE_int64(socket_send_timeout, 0, "socket send timeout in millisecond. \
0: using system default value.");
DEFINE_int64(socket_recv_timeout, 0, "socket recv timeout in millisecond. \
0: using system default value.");
DEFINE_int32(socket_send_buf_size, 0, "socket send buffer size in Bytes. \
0: using system default value.");
DEFINE_int32(socket_recv_buf_size, 0, "socket recv buffer size in Bytes. \
0: using system default value.");
DEFINE_int32(reconnect_interval, 2000, "socket reconnect interval time in millisecond.");
DEFINE_bool(auto_reconnect, true, "enable/disable socket reconnect.");
DEFINE_int32(ping_time_out, 2000, "Ping timeout in millisecond");

namespace PocoRpc {

PocoRpcChannel::PocoRpcChannel(const std::string& host, uint16 port) :
exit_(true), connected_(false), re_connect_times_(0),
rpc_sending_(NULL), buf_sending_(NULL), buf_recving_(NULL), socket_(NULL),
on_reconnect_faild_cb_(NULL), onWritable_ready_(false) {
  auto_reconnect_ = FLAGS_auto_reconnect;
  uuid_ = Poco::UUIDGenerator::defaultGenerator().createRandom().toString();
  rpc_pending_.reset(new RpcControllerQueue());
  rpc_waiting_.reset(new PocoRpcControllerMap());
  mutex_waiting_response_.reset(new Poco::FastMutex());
  recv_buf_array_.reset(new BytesBufferQueue());
  mutex_recv_buf_array_.reset(new Poco::FastMutex());
  address_.reset(new Poco::Net::SocketAddress(host, port));

  reconnect_worker_.reset(new Poco::Thread());
  reconnect_cont_.reset(new Poco::Condition());
  mutex_reconnect_cont_.reset(new Poco::FastMutex());
  reconnect_func_.reset(Poco::NewPermanentCallback(this,
          &PocoRpcChannel::auto_reconnect));

  mutex_onWritable_ready_.reset(new Poco::FastMutex());
}

PocoRpcChannel::~PocoRpcChannel() {
  if (not exit_) {
    Exit();
  }
  rpc_pending_->clear();
  rpc_waiting_->clear();
  STLClear(recv_buf_array_.get());
}

/// must be called after constructor

bool PocoRpcChannel::Init() {
  net_worker_.reset(new Poco::Thread());
  response_worker_.reset(new Poco::Thread());
  ra_response_.reset(Poco::NewPermanentCallback(this, &PocoRpcChannel::process_response));

  reactor_.reset(new PocoRpcSocketReactor());
  // net_worker_, response_worker_ 2个线程在构造函数里启动, 在析构函数里stop
  net_worker_->start(*reactor_);
  response_worker_->start(*ra_response_);
  exit_ = false;

  bool ret = Connect();
  if (ret) {
    // 连接成功, 则启动自动重连的线程, 用于检查socket是否连接, 在没有连接的时候
    // 自动重连
    reconnect_worker_->start(*reconnect_func_);
  }
  return ret;
}

/**
 * Call the given method of the remote service.  The signature of this
 * procedure looks the same as Service::CallMethod(), but the requirements
 * are less strict in one important way:  the request and response objects
 * need not be of any specific class as long as their descriptors are
 * method->input_type() and method->output_type().
 * PocoRpcChannel obj will take over RpcController object
 * 
 * @param method
 * @param controller
 * @param request
 * @param response
 * @param done
 */
void PocoRpcChannel::CallMethod(const google::protobuf::MethodDescriptor* method,
        google::protobuf::RpcController* controller,
        const google::protobuf::Message* request,
        google::protobuf::Message* response,
        google::protobuf::Closure* done) {
  PocoRpcController* rpc_controller = dynamic_cast<PocoRpcController*> (controller);
  CHECK(rpc_controller != NULL) << "RpcController must be class: PocoRpcController";
  rpc_controller->set_method_desc(method);
  rpc_controller->set_request(request);
  rpc_controller->set_response(response);
  rpc_controller->set_on_done_callback(done);

  AutoPocoRpcControllerPtr aptr_rpc_ctrl;
  aptr_rpc_ctrl.assign(rpc_controller, true);
  aptr_rpc_ctrl->SetFailed("Waiting for sending");
  rpc_pending_->push(aptr_rpc_ctrl);
}

/**
 * Exit() 方法只能被调用一次.
 */
void PocoRpcChannel::Exit() {
  CHECK(exit_ == false);
  exit_ = true;
  rpc_pending_->clear_on_popuped_callback();
  rpc_pending_->clear_on_pushed_callback();
  reconnect_worker_->join();
  reactor_->stop();
  net_worker_->join();
  if (socket_.get() != NULL) {
    unreg_reactor_handler(socket_.get());
  }

  response_worker_->join(); // stopped by set exit_=true

  cancel_waiting_response_rpc("Normal exit");
}

void PocoRpcChannel::set_auto_reconnect(bool auto_reconnect) {
  auto_reconnect_ = auto_reconnect;
}

uint32 PocoRpcChannel::get_re_connect_times() {
  return re_connect_times_;
}

/**
 * 设置当重新连接失败时会调用的callback. callback函数里可以通过
 * get_re_connect_times() 函数得到已经尝试重连多少次. 可以通过
 * set_auto_reconnect(true/false) 来设置是否继续重连
 * @param callback
 */
void PocoRpcChannel::NotifyOnReConnectFaild(google::protobuf::Closure* callback) {
  on_reconnect_faild_cb_.reset(callback);
}

const std::string& PocoRpcChannel::get_uuid() {
  return uuid_;
}

std::string PocoRpcChannel::DebugString() {
  std::stringstream ss;
  ss << "{" << std::endl;
  ss << "rpc_waiting_ : {" << std::endl;
  {
    Poco::ScopedLock<Poco::FastMutex> lock(*mutex_waiting_response_);
    PocoRpcControllerMap::iterator it = rpc_waiting_->begin();
    for (; it != rpc_waiting_->end(); ++it) {
      ss << it->second->DebugString() << "," << std::endl;
    }
  }
  ss << "}" << std::endl;
  ss << "rpc_pending_ : {" << std::endl;
  rpc_pending_->lock();
  RpcControllerQueue::iterator it_pending = rpc_pending_->begin();
  for (; it_pending != rpc_pending_->end(); ++it_pending) {
    AutoPocoRpcControllerPtr ptr = *it_pending;
    ss << ptr->DebugString() << "," << std::endl;
  }
  rpc_pending_->unlock();
  ss << "}" << std::endl;
  ss << "rpc_sending_ : " << (rpc_sending_.isNull() ? "NULL" : rpc_sending_->DebugString()) << std::endl;
  ss << "SocketAddress : " << address_->toString() << std::endl;
  ss << "}";
  return ss.str();
}

AutoPocoRpcControllerPtr PocoRpcChannel::NewRpcController() {
  AutoPocoRpcControllerPtr ptr(new PocoRpcController(this));
  return ptr;
}

bool PocoRpcChannel::Connect() {
  CHECK(connected_ == false);
  CHECK(onWritable_ready_ == false);
  CHECK(socket_.get() == NULL);
  try {
    socket_.reset(CreateSocket());
  } catch (Poco::Exception ex) {
    LOG(ERROR) << "Faild to connect rpc server: " << address_->toString() <<
            " Exception: " << ex.message();
    if (socket_.get() != NULL) {
      socket_->close();
      socket_.release();
    }
    connected_ = false;
    return false;
  }
  // successed cennected with rpc server
  connected_ = true;
  rpc_pending_->clear_on_popuped_callback();
  rpc_pending_->clear_on_pushed_callback();

  reg_reactor_handler(socket_.get());
  
  Poco::Runnable* on_pushed_cb = Poco::NewPermanentCallback(this,
          &PocoRpcChannel::on_pushed_rpc);
  Poco::Runnable* on_popuped_cb = Poco::NewPermanentCallback(this,
          &PocoRpcChannel::on_popup_rpc);
  rpc_pending_->reg_on_pushed_callback(on_pushed_cb);
  rpc_pending_->reg_on_popuped_callback(on_popuped_cb);

  scoped_ptr<BaseService_Stub> bservice(new BaseService_Stub(this));
  AutoPocoRpcControllerPtr ping_ctr = NewRpcController();
  PingReq req;
  PingReply reply;

  bservice->Ping(ping_ctr.get(), &req, &reply, NULL);
  CHECK(onWritable_ready_);
  ping_ctr->tryWait(FLAGS_ping_time_out);
  if (reply.status() != E_OK) {
    LOG(ERROR) << "Ping server faild.";
    rpc_pending_->clear_on_popuped_callback();
    rpc_pending_->clear_on_pushed_callback();

    // 可能在之前的过程中, 调用了on_socket_error(), 则就不需要再次unreg_reactor_handler
    // 通过socket_.get() 是否为NULL 来进行判断
    if (socket_.get() != NULL) {
      unreg_reactor_handler(socket_.get());
      socket_->close();
      socket_.release();
    }
    connected_ = false;
    return false;
  }
  return true;
}

/**
 * 根据rpc_id, 将PocoRpcChannel 内部几个队列中, id相同的RpcController 删除掉
 * 这个方法应该由 PocoRpcController::StartCancel() 方法调用. Rpc框架的使用者
 * 不应该使用此方法.
 * 
 * @param rpc_id
 */
void PocoRpcChannel::RemoveCanceledRpc(uint64 rpc_id) {
  // rpc 的request尚未开始发送, 所以直接从 rpc_pending_ 队列中删除
  rpc_pending_->lock();
  RpcControllerQueue::iterator it_pending = rpc_pending_->begin();

  for (; it_pending != rpc_pending_->end(); ++it_pending) {
    if ((*it_pending)->id() == rpc_id) {
      CHECK((*it_pending)->IsCanceled()) << "RPC_ID:" << rpc_id << "is not canceled.";
      rpc_pending_->erase(it_pending);
      break;
    }
  }
  rpc_pending_->unlock();

  /// 在 rpc_pending_ 队列里没有找到, 则继续在 rpc_waiting_ 队列里找
  {
    Poco::ScopedLock<Poco::FastMutex> lock(*mutex_waiting_response_);
    PocoRpcControllerMap::iterator it_waiting = rpc_waiting_->find(rpc_id);
    if (it_waiting != rpc_waiting_->end()) { // 找到对应的
      CHECK(it_waiting->second->IsCanceled()) << "RPC_ID:" << rpc_id << "is not canceled.";
      rpc_waiting_->erase(it_waiting);
    }
  }

  // 要删除的Rpc是正在发送Request的过程中, 但是又没有100% 完成发送的
  // 只有等待继续完成发送后, rpc 移动到 rpc_waiting_ 队列里, 但是rpc标记为
  // canceled
  if (not rpc_sending_.isNull() && rpc_sending_->id() == rpc_id) {
    CHECK(rpc_sending_->IsCanceled()) << "RPC_ID:" << rpc_id << "is not canceled.";
    //    rpc_sending_->mark_canceled();
  }
}

Poco::Net::StreamSocket* PocoRpcChannel::CreateSocket() {
  // @todo 增加flags, 提供参数设置socket的选项
  Poco::Net::StreamSocket* sock = new Poco::Net::StreamSocket();
  sock->connect(*address_);
  PLOG(INFO) << "******** connect socket_fd=" << sock->impl()->sockfd() <<
          " erron=" << errno;
  sock->setBlocking(false);
  sock->setKeepAlive(true);
  sock->setNoDelay(true);
  sock->setReuseAddress(true);
  sock->setReusePort(true);

  if (FLAGS_socket_recv_timeout > 0) {
    sock->setReceiveTimeout(Poco::Timespan(FLAGS_socket_recv_timeout));
  }
  if (FLAGS_socket_send_timeout > 0) {
    sock->setSendTimeout(Poco::Timespan(FLAGS_socket_send_timeout));
  }
  if (FLAGS_socket_recv_buf_size > 0) {
    sock->setReceiveBufferSize(FLAGS_socket_recv_buf_size);
  }
  if (FLAGS_socket_send_buf_size > 0) {
    sock->setSendBufferSize(FLAGS_socket_send_buf_size);
  }
  return sock;
}

void PocoRpcChannel::reg_on_writeable(Poco::Net::StreamSocket* sock) {
  Poco::ScopedLock<Poco::FastMutex> lock(*mutex_onWritable_ready_);
  if (onWritable_ready_) {
    return;
  }
  reactor_->addEventHandler(*sock,
          Poco::NObserver<PocoRpcChannel, Poco::Net::WritableNotification>(*this,
          &PocoRpcChannel::onWritable));
  onWritable_ready_ = true;
}

void PocoRpcChannel::unreg_on_writeable(Poco::Net::StreamSocket* sock) {
  Poco::ScopedLock<Poco::FastMutex> lock(*mutex_onWritable_ready_);
  if (not onWritable_ready_) {
    return;
  }
  reactor_->removeEventHandler(*sock,
          Poco::NObserver<PocoRpcChannel, Poco::Net::WritableNotification>(*this,
          &PocoRpcChannel::onWritable));
  onWritable_ready_ = false;
}

void PocoRpcChannel::reg_reactor_handler(Poco::Net::StreamSocket* sock) {
  reactor_->addEventHandler(*sock,
          Poco::NObserver<PocoRpcChannel, Poco::Net::ReadableNotification>(*this,
          &PocoRpcChannel::onReadable));
  reg_on_writeable(sock);
  reactor_->addEventHandler(*sock,
          Poco::NObserver<PocoRpcChannel, Poco::Net::ShutdownNotification>(*this,
          &PocoRpcChannel::onShutdown));
  reactor_->addEventHandler(*sock,
          Poco::NObserver<PocoRpcChannel, Poco::Net::ErrorNotification>(*this,
          &PocoRpcChannel::onError));
  LOG(INFO) << "########## reg_reactor_handler fd=" << sock->impl()->sockfd() <<
          " ref_count=" << sock->impl()->referenceCount();
}

void PocoRpcChannel::unreg_reactor_handler(Poco::Net::StreamSocket* sock) {
  reactor_->removeEventHandler(*sock,
          Poco::NObserver<PocoRpcChannel, Poco::Net::ReadableNotification>(*this,
          &PocoRpcChannel::onReadable));
  unreg_on_writeable(sock);
  reactor_->removeEventHandler(*sock,
          Poco::NObserver<PocoRpcChannel, Poco::Net::ShutdownNotification>(*this,
          &PocoRpcChannel::onShutdown));
  reactor_->removeEventHandler(*sock,
          Poco::NObserver<PocoRpcChannel, Poco::Net::ErrorNotification>(*this,
          &PocoRpcChannel::onError));
  LOG(INFO) << "########## unreg_reactor_handler fd=" << sock->impl()->sockfd() <<
          " ref_count=" << sock->impl()->referenceCount();
}

uint32 get_rpc_msg_size(Poco::Net::StreamSocket* sock) {
  uint32 buf;
  int recv_size = sock->receiveBytes(reinterpret_cast<void*> (&buf), 4);
  CHECK(recv_size == 4);
  return ntohl(buf);
}

void PocoRpcChannel::onReadable(const Poco::AutoPtr<Poco::Net::ReadableNotification>& pNf) {
  LOG(INFO) << "socket is readable";
  if (socket_->available() == 0) {
    LOG(INFO) << "socket is readable status, but it's available data length == 0, so close this socket.";
    on_socket_error();
    return;
  }

  if (buf_recving_.get() == NULL) {
    try {
      if (socket_->available() < 4) {
        // socket buffer 里不够4字节, 则等待下次
        LOG(INFO) << "socket buffer 里不够4字节, 则等待下次";
        return;
      }

      // 先读4个字节头部, 得到body的size
      char buf[4];
      int recv_size = socket_->receiveBytes(reinterpret_cast<void*> (buf), 4);
      if (recv_size == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) {
          LOG(WARNING) << "socket is EAGAIN or EWOULDBLOCK or EINTR";
          return;
        } else {
          LOG(ERROR) << "socket recv ERROR: " << errno;
          on_socket_error();
          return;
        }
      }

      if (recv_size == 0) {
        // 0 means socket was shutdown by server side.
        on_socket_error();
        return;
      }
      CHECK(recv_size == 4) << "读头部4个字节的size出错";
      uint32* puint32 = (uint32*) buf;
      uint32 buf_size = ntohl(*puint32);
      buf_recving_.reset(new BytesBuffer(buf_size));
      buf_recving_->set_done_size(4);

    } catch (Poco::Exception ex) {
      LOG(ERROR) << ex.message();
      on_socket_error();
      return;
    }
  }
  // buf_recving_ ready
  try {
    uint32 recv_size = socket_->receiveBytes(
            reinterpret_cast<void*> (buf_recving_->phead() + buf_recving_->get_done_size()),
            (buf_recving_->get_total_size() - buf_recving_->get_done_size())
            );
    if (recv_size == -1) {
      if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) {
        LOG(WARNING) << "socket is EAGAIN or EWOULDBLOCK or EINTR";
        return;
      } else {
        LOG(ERROR) << "socket recv ERROR: " << errno;
        on_socket_error();
        return;
      }
    }

    if (recv_size == 0) {
      // 0 means socket was shutdown by server side.
      on_socket_error();
      return;
    }

    buf_recving_->set_done_size(buf_recving_->get_done_size() + recv_size);
    LOG(INFO) << "recv_size = " << recv_size;
    if (buf_recving_->get_done_size() == buf_recving_->get_total_size()) {
      // response buf 接收 100%
      Poco::ScopedLock<Poco::FastMutex> lock(*mutex_recv_buf_array_);
      LOG(INFO) << "完整接收 Response";
      recv_buf_array_->push(buf_recving_.release()); // 小心, 不能使用Reset() 方法
    }
  } catch (Poco::Exception ex) {
    LOG(ERROR) << ex.message();
    on_socket_error();
    return;
  }
}

void PocoRpcChannel::onWritable(const Poco::AutoPtr<Poco::Net::WritableNotification>& pNf) {
  Poco::AutoPtr<Poco::Net::WritableNotification>& tmpNf = const_cast<Poco::AutoPtr<Poco::Net::WritableNotification>&> (pNf);
  LOG(INFO) << "socket is Writable. socket_fd=" << tmpNf->socket().impl()->sockfd();
  if (rpc_sending_.isNull()) {
    // 从 rpc_pending_ 获取一个AutoPocoRpcControllerPtr, 并且 IsCanceled == false
    AutoPocoRpcControllerPtr tmp_rpc(NULL);
    rpc_pending_->tryPopup(&tmp_rpc, 0);
    if (tmp_rpc.isNull()) {
      /// rpc_pending_ 队列里没有需要发送的request, 推出, 等待下一次
      /// onWritable 操作
      return;
    } else {
      if (tmp_rpc->IsCanceled()) {
        // rpc is canceled, so ignore it and continue to get next one
        return;
      } else { // not canceled
        rpc_sending_.assign(tmp_rpc);
        rpc_sending_->SetFailed("Sending");
        buf_sending_.reset(rpc_sending_->NewBytesBuffer());
      }
    }
  }

  try {
    // buf_sending_ ready
    CHECK(buf_sending_.get() != NULL) << "rpc_sending_ = " << rpc_sending_.get();
    void* pbuf = reinterpret_cast<void*> (buf_sending_->phead() + buf_sending_->get_done_size());
    int left_size = buf_sending_->get_total_size() - buf_sending_->get_done_size();
    int send_size = socket_->sendBytes(pbuf, left_size);
    if (send_size == -1) {
      if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) {
        LOG(WARNING) << "socket is EAGAIN or EWOULDBLOCK or EINTR";
        return;
      } else {
        LOG(ERROR) << "socket send ERROR: " << errno;
        on_socket_error();
        return;
      }
    }

    buf_sending_->set_done_size(buf_sending_->get_done_size() + send_size);

    if (buf_sending_->get_done_size() == buf_sending_->get_total_size()) {
      // 当前buf的所有数据发送完成 100%, rpc 移动到rpc_waiting_ 队列里.
      Poco::ScopedLock<Poco::FastMutex> lock(*mutex_waiting_response_);
      LOG(INFO) << "Request sended: " << rpc_sending_->request_->GetDescriptor()->full_name();
      rpc_waiting_->insert(std::pair<uint64, AutoPocoRpcControllerPtr>(
              rpc_sending_->id(),
              rpc_sending_));
      rpc_sending_->SetFailed("Waiting for response");
      rpc_sending_.assign(NULL);
      buf_sending_.release();
    }
  } catch (Poco::Exception ex) {
    LOG(ERROR) << ex.message();
    on_socket_error();
    return;
  }
}

void PocoRpcChannel::onShutdown(const Poco::AutoPtr<Poco::Net::ShutdownNotification>& pNf) {
  // do nothing
  LOG(INFO) << "Shutdown SOCKET";
}

void PocoRpcChannel::onError(const Poco::AutoPtr<Poco::Net::ErrorNotification>& pNf) {
  LOG(ERROR) << pNf->name();
}

void PocoRpcChannel::process_response() {
  while (true) {
    if (exit_) return;
    BytesBuffer* recved_buf = NULL;
    {
      Poco::ScopedLock<Poco::FastMutex> lock(*mutex_recv_buf_array_);
      if (recv_buf_array_->size() > 0) {
        LOG(INFO) << "recv_buf_array_->size()=" << recv_buf_array_->size();
      }
      recv_buf_array_->tryPopup(&recved_buf, 0);
    }
    if (recved_buf != NULL) {
      scoped_ptr<RpcMessage> rpc_msg(new RpcMessage());

      if (not rpc_msg->ParseFromArray(recved_buf->pbody(),
              recved_buf->get_body_size())) {
        LOG(ERROR) << "RpcMessage 反序列化出错";
        on_socket_error();
        continue;
      }

      LOG(INFO) << rpc_msg->DebugString();

      AutoPocoRpcControllerPtr rpc_ctrl = NULL;
      {
        Poco::ScopedLock<Poco::FastMutex> lock_rpc_waiting_(*mutex_waiting_response_);
        PocoRpcControllerMap::iterator it_rpc = rpc_waiting_->find(rpc_msg->id());
        if (it_rpc != rpc_waiting_->end()) {
          rpc_ctrl = it_rpc->second;
          rpc_waiting_->erase(it_rpc);
        }
      }
      if (not rpc_ctrl.isNull() && not rpc_ctrl->IsCanceled()) {
        CHECK(rpc_ctrl->response_ != NULL);
        bool ret = rpc_ctrl->response_->ParseFromString(rpc_msg->message_body());
        if (!ret) {
          rpc_ctrl->SetFailed("Response 反序列化出错");
        }
        rpc_ctrl->signal_rpc_over();
      }
    } else {
      Poco::Thread::sleep(150);
    }

    // rpc_waiting_ 里可能会存在一些已经标记成Canceled的Rpc, 找到并将其从
    // rpc_waiting_ 中删除. 每次大循环(最外面的while循环)找到一个被标记成Canceled
    // 的, 就删除这个. 剩余的, 留到下次循环删除. 多循环几次, 自然就都删除掉了
    // 为了避免每次大循环的时候都去遍历一次rpc_waiting_ , 所以就检查recv_buf_array_
    // 是否为空. 为空意味着空闲, 就开始下面的回收处理
    if (recv_buf_array_->empty()) {
      Poco::ScopedLock<Poco::FastMutex> lock_rpc_waiting_(*mutex_waiting_response_);
      PocoRpcControllerMap::iterator it_rpc = rpc_waiting_->begin();
      for (; it_rpc != rpc_waiting_->end(); ++it_rpc) {
        if (it_rpc->second->IsCanceled()) {
          rpc_waiting_->erase(it_rpc);
          break;
        }
      }
    }
  }
}

/**
 * Cancel 掉 rpc_waiting_ 里所有已经发送Request, 等待response的rpc.
 * 当socket 发生错误的时候, 或者是程序要退出, 调用exit()方法的时候, 调用此方法
 * 将waiting 状态的rpc都cancel掉
 * @param reason : cancel的原因
 */
void PocoRpcChannel::cancel_waiting_response_rpc(const std::string& reason) {
  scoped_ptr< std::vector<AutoPocoRpcControllerPtr> > tmp_rpc_list(
          new std::vector<AutoPocoRpcControllerPtr>());
  {
    Poco::ScopedLock<Poco::FastMutex> lock(*mutex_waiting_response_);
    PocoRpcControllerMap::iterator it = rpc_waiting_->begin();
    for (; it != rpc_waiting_->end(); ++it) {
      tmp_rpc_list->push_back(it->second);
    }
  }

  std::vector<AutoPocoRpcControllerPtr>::iterator it_tobe_canceled = tmp_rpc_list->begin();
  for (; it_tobe_canceled != tmp_rpc_list->end(); ++it_tobe_canceled) {
    AutoPocoRpcControllerPtr rpc_ctrl = *it_tobe_canceled;
    rpc_ctrl->SetFailed(reason);
    rpc_ctrl->StartCancel();
  }
  tmp_rpc_list->clear();
}

/// on_socket_error() 是在reactor的线程上被调用的, 所以不会有并发的问题.
/// 但是它可能会被多次调用. 所以要进行一些检查, 要采取防御性编码

void PocoRpcChannel::on_socket_error() {
  LOG(INFO) << "on_socket_error";
  CHECK(socket_.get() != NULL) << "只有socket 发送错误的时候才会调用此方法, 因此socket_不应该为NULL";
  rpc_pending_->clear_on_popuped_callback();
  rpc_pending_->clear_on_pushed_callback();
  // 2. 取消reactor 对此socket进行select/poll检测, 关闭socket, 
  unreg_reactor_handler(socket_.get());
  socket_->close();
  socket_.release();
  connected_ = false;


  // 3. cancel 掉所有已经发送Request, 等待Response的Rpc. 因为socket断掉后,
  //    这些rpc已经没有机会再收到Response了. 
  // @todo 如果以后实现了client_id, 服务器端根据client_id向客户端继续发送之前
  // 没有发送的的response就可以不必cancel_waiting_response_rpc
  cancel_waiting_response_rpc("socket error");

  // 4. 丢弃正在接收状态(未能100%完成接收)的 ByteBuffer
  buf_recving_.release();

  // 5. 重置正在发送Request状态中(未能100%完成发送)的rpc
  if (not rpc_sending_.isNull()) {
    // 通过设置 done_size 为0, 等socket重新连接后, 就可以重新向server端
    // 发送Request了
    buf_sending_->set_done_size(0);
  }

  // TODO 增加处理自动重连的代码
  reconnect_cont_->signal();
}

void PocoRpcChannel::auto_reconnect() {
  LOG(INFO) << "Start auto_reconnect thread...";
  while (true) {
    if (exit_) {
      LOG(INFO) << "Exit auto_reconnect thread.";
      return;
    }
    if (not connected_) {
      LOG(INFO) << "Try to reconnect to server";
      bool ok = Connect();
      re_connect_times_++;
      if (ok) {
        re_connect_times_ = 0; // 重连成功, 则计数归零
        LOG(INFO) << "Reconnect OK.";
        continue;
      } else {
        // 重连失败, 则调用指定的callback
        // 可以在callback函数里检查已经重连的次数, 通过set_auto_reconnect(false)
        // 取消重连.
        LOG(INFO) << "Reconnect Faild.";
        if (on_reconnect_faild_cb_.get() != NULL) {
          on_reconnect_faild_cb_->Run();
        }
        Poco::Thread::sleep(FLAGS_reconnect_interval);
      }
    } else {
      // socket is connected now, tryWait for sign
      reconnect_cont_->tryWait(*mutex_reconnect_cont_, 2000);
    }

  }


//  if (connected_) return;
//  CHECK(socket_.get() == NULL);
//  while (auto_reconnect_) {
//    bool ok = Connect();
//    re_connect_times_++;
//    if (ok) {
//      re_connect_times_ = 0; // 重连成功, 则计数归零
//      LOG(INFO) << "Reconnect OK.";
//      return;
//    } else {
//      // 重连失败, 则调用指定的callback
//      // 可以在callback函数里检查已经重连的次数, 通过set_auto_reconnect(false)
//      // 取消重连.
//      if (on_reconnect_faild_cb_.get() != NULL) {
//        on_reconnect_faild_cb_->Run();
//      }
//    }
//    Poco::Thread::sleep(FLAGS_reconnect_interval);
//  }
}

void PocoRpcChannel::on_pushed_rpc() {
  if (rpc_pending_->size() == 1) {
    if (socket_.get() != NULL) {
      reg_on_writeable(socket_.get());
      LOG(INFO) << "****************************** reg_on_writeable";
    }
  }
}

void PocoRpcChannel::on_popup_rpc() {
  if (rpc_pending_->size() == 0) {
    if (socket_.get() != NULL) {
      unreg_on_writeable(socket_.get());
      LOG(INFO) << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ unreg_on_writeable";
    }
  }
}

} // namespace
