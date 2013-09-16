/* 
 * File:   PocoRpcChannel.cc
 * Author: kk
 * 
 * Created on 2013年8月12日, 下午7:33
 */

#include "rpclib/PocoRpcController.h"
#include "rpclib/PocoRpcChannel.h"
#include "rpclib/BytesBuffer.h"
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
DEFINE_int32(reconnect_interval, 200, "socket reconnect interval time in millisecond.");

namespace PocoRpc {

PocoRpcChannel::PocoRpcChannel(const std::string& host, uint16 port) :
exit_(true), connected_(false), re_connect_times_(0), auto_reconnect_(true),
rpc_sending_(NULL), buf_sending_(NULL), socket_(NULL),
on_reconnect_faild_cb_(NULL) {
  uuid_ = Poco::UUIDGenerator::defaultGenerator().createRandom().toString();
  rpc_pending_.reset(new RpcControllerQueue());
  rpc_waiting_.reset(new PocoRpcControllerMap());
  mutex_waiting_response_.reset(new Poco::FastMutex());
  recv_buf_array_.reset(new BytesBufferQueue());
  mutex_recv_buf_array_.reset(new Poco::FastMutex());
  address_.reset(new Poco::Net::SocketAddress(host, port));
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

void PocoRpcChannel::init() {
  net_worker_.reset(new Poco::Thread());
  response_worker_.reset(new Poco::Thread());
  ra_response_.reset(Poco::NewPermanentCallback(this, &PocoRpcChannel::process_response));

  reactor_.reset(new Poco::Net::SocketReactor());
  // net_worker_, response_worker_ 2个线程在构造函数里启动, 在析构函数里stop
  net_worker_->start(*reactor_);
  response_worker_->start(*ra_response_);
  exit_ = false;
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
  rpc_pending_->push(aptr_rpc_ctrl);
}

bool PocoRpcChannel::Connect() {
  CHECK(connected_ == false);
  try {
    socket_.reset(CreateSocket());
    socket_->connectNB(*address_);
  } catch (Poco::Exception ex) {
    LOG(ERROR) << "Faild to connect rpc server: " << address_->toString() <<
            " Exception: " << ex.message();
    socket_->shutdown();
    socket_.reset(NULL);
    connected_ = false;
    return false;
  }
  // successed cennected with rpc server
  connected_ = true;
  reg_reactor_handler(socket_.get());
  return true;
}

/**
 * Exit() 方法只能被调用一次.
 */
void PocoRpcChannel::Exit() {
  CHECK(exit_ == false);
  exit_ = true;
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
  ss << "PocoRpcControllerMap : {" << std::endl;
  PocoRpcControllerMap::iterator it = rpc_waiting_->begin();
  for (; it != rpc_waiting_->end(); ++it) {
    ss << it->second->DebugString() << "," << std::endl;
  }

  ss << "}" << std::endl;
  ss << "SocketAddress : " << address_->toString() << std::endl;
  ss << "}";
  return ss.str();
}

AutoPocoRpcControllerPtr PocoRpcChannel::NewRpcController() {
  AutoPocoRpcControllerPtr ptr(new PocoRpcController(this));
  return ptr;
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
  Poco::Net::StreamSocket* sock = new Poco::Net::StreamSocket(*address_);
  sock->setBlocking(false);
  sock->setKeepAlive(true);
  sock->setNoDelay(true);
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
}

void PocoRpcChannel::reg_reactor_handler(Poco::Net::StreamSocket* sock) {
  reactor_->addEventHandler(*sock,
          Poco::NObserver<PocoRpcChannel, Poco::Net::ReadableNotification>(*this,
          &PocoRpcChannel::onReadable));
  reactor_->addEventHandler(*sock,
          Poco::NObserver<PocoRpcChannel, Poco::Net::WritableNotification>(*this,
          &PocoRpcChannel::onWritable));
  reactor_->addEventHandler(*sock,
          Poco::NObserver<PocoRpcChannel, Poco::Net::ShutdownNotification>(*this,
          &PocoRpcChannel::onShutdown));
  reactor_->addEventHandler(*sock,
          Poco::NObserver<PocoRpcChannel, Poco::Net::ErrorNotification>(*this,
          &PocoRpcChannel::onError));
}

void PocoRpcChannel::unreg_reactor_handler(Poco::Net::StreamSocket* sock) {
  reactor_->removeEventHandler(*sock,
          Poco::NObserver<PocoRpcChannel, Poco::Net::ReadableNotification>(*this,
          &PocoRpcChannel::onReadable));
  reactor_->removeEventHandler(*sock,
          Poco::NObserver<PocoRpcChannel, Poco::Net::WritableNotification>(*this,
          &PocoRpcChannel::onWritable));
  reactor_->removeEventHandler(*sock,
          Poco::NObserver<PocoRpcChannel, Poco::Net::ShutdownNotification>(*this,
          &PocoRpcChannel::onShutdown));
  reactor_->removeEventHandler(*sock,
          Poco::NObserver<PocoRpcChannel, Poco::Net::ErrorNotification>(*this,
          &PocoRpcChannel::onError));
}

uint32 get_rpc_msg_size(Poco::Net::StreamSocket* sock) {
  uint32 buf;
  int recv_size = sock->receiveBytes(reinterpret_cast<void*> (&buf), 4);
  CHECK(recv_size == 4);
  return ntohl(buf);
}

void PocoRpcChannel::onReadable(const Poco::AutoPtr<Poco::Net::ReadableNotification>& pNf) {
  if (buf_recving_.get() == NULL) {
    try {
      if (socket_->available() < 4) {
        // socket buffer 里不够4字节, 则等待下次
        return;
      }

      // 先读4个字节头部, 得到body的size
      uint32 buf;
      int recv_size = socket_->receiveBytes(reinterpret_cast<void*> (&buf), 4);
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
      uint32 buf_size = ntohl(buf);
      buf_recving_.reset(new BytesBuffer(buf_size));

    } catch (Poco::Exception ex) {
      LOG(ERROR) << ex.message();
      on_socket_error();
      return;
    }
  }
  // buf_recving_ ready
  try {
    uint32 recv_size = socket_->receiveBytes(
            reinterpret_cast<void*> (buf_recving_->pbody() + buf_recving_->get_done_size()),
            (buf_recving_->get_size() - buf_recving_->get_done_size())
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
    if (buf_recving_->get_done_size() == buf_recving_->get_size()) {
      // response buf 接收 100%
      Poco::ScopedLock<Poco::FastMutex> lock(*mutex_recv_buf_array_);
      recv_buf_array_->push(buf_recving_.release()); // 小心, 不能使用Reset() 方法
    }
  } catch (Poco::Exception ex) {
    LOG(ERROR) << ex.message();
    on_socket_error();
    return;
  }
}

void PocoRpcChannel::onWritable(const Poco::AutoPtr<Poco::Net::WritableNotification>& pNf) {
  if (rpc_sending_.isNull()) {
    // 从 rpc_pending_ 获取一个AutoPocoRpcControllerPtr, 并且 IsCanceled == false
    while (true) {
      if (exit_) {
        break;
      }
      AutoPocoRpcControllerPtr tmp_rpc(NULL);
      rpc_pending_->tryPopup(&tmp_rpc, 0);
      if (tmp_rpc.isNull()) {
        /// rpc_pending_ 队列里没有需要发送的request, 推出, 等待下一次
        /// onWritable 操作
        return;
      } else {
        if (tmp_rpc->IsCanceled()) {
          // rpc is canceled, so ignore it and continue to get next one
          continue;
        } else { // not canceled
          rpc_sending_.assign(tmp_rpc);
          buf_sending_.reset(rpc_sending_->NewBytesBuffer());
          break;
        }
      }
    }
  }

  try {
    // buf_sending_ ready
    void* pbuf = reinterpret_cast<void*> (buf_sending_->pbody() + buf_sending_->get_done_size());
    int left_size = buf_sending_->get_size() - buf_sending_->get_done_size();
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

    if (send_size == left_size) {
      // 当前buf的所有数据发送完成 100%, rpc 移动到rpc_waiting_ 队列里.
      Poco::ScopedLock<Poco::FastMutex> lock(*mutex_waiting_response_);
      rpc_waiting_->insert(std::pair<uint64, AutoPocoRpcControllerPtr>(
              rpc_sending_->id(),
              rpc_sending_));
      rpc_sending_.assign(NULL);
      buf_sending_.reset(NULL);
    } else {
      // 没有发送完, 等待下次发送剩余数据. 更新done_size
      buf_sending_->set_done_size(buf_sending_->get_done_size() + send_size);
    }

  } catch (Poco::Exception ex) {
    LOG(ERROR) << ex.message();
    on_socket_error();
    return;
  }
}

void PocoRpcChannel::onShutdown(const Poco::AutoPtr<Poco::Net::ShutdownNotification>& pNf) {
  // do nothing
}

void PocoRpcChannel::onError(const Poco::AutoPtr<Poco::Net::ErrorNotification>& pNf) {
  LOG(ERROR) << pNf->name();
}

void PocoRpcChannel::process_response() {
  while (true) {
    if (exit_) break;
    BytesBuffer* recved_buf = NULL;
    {
      Poco::ScopedLock<Poco::FastMutex> lock(*mutex_recv_buf_array_);
      recv_buf_array_->tryPopup(&recved_buf, 100);
    }
    if (recved_buf != NULL) {
      scoped_ptr<RpcMessage> rpc_msg(new RpcMessage());

      if (not rpc_msg->ParseFromArray(recved_buf->pbody(),
              recved_buf->get_size())) {
        LOG(ERROR) << "RpcMessage 反序列化出错";
        continue;
      }
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
        rpc_ctrl->signal_rpc_over();
      }
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
  CHECK(socket_.get() != NULL) << "只有socket 发送错误的时候才会调用此方法, 因此socket_不应该为NULL";

  // 2. 取消reactor 对此socket进行select/poll检测, 关闭socket, 
  unreg_reactor_handler(socket_.get());
  socket_->shutdown();
  socket_.reset(NULL);
  connected_ = false;


  // 3. cancel 掉所有已经发送Request, 等待Response的Rpc. 因为socket断掉后,
  //    这些rpc已经没有机会再收到Response了. 
  // @todo 如果以后实现了client_id, 服务器端根据client_id向客户端继续发送之前
  // 没有发送的的response就可以不必cancel_waiting_response_rpc
  cancel_waiting_response_rpc("socket error");

  // 4. 丢弃正在接收状态(未能100%完成接收)的 ByteBuffer
  buf_recving_.reset(NULL);

  // 5. 重置正在发送Request状态中(未能100%完成发送)的rpc
  if (not rpc_sending_.isNull()) {
    // 通过设置 done_size 为0, 等socket重新连接后, 就可以重新向server端
    // 发送Request了
    buf_sending_->set_done_size(0);
  }

  // TODO 增加处理自动重连的代码
  auto_reconnect();
}

void PocoRpcChannel::auto_reconnect() {
  if (connected_) return;
  CHECK(socket_.get() == NULL);
  while (auto_reconnect_) {
    bool ok = Connect();
    re_connect_times_++;
    if (ok) {
      re_connect_times_ = 0; // 重连成功, 则计数归零
      break;
    } else {
      // 重连失败, 则调用指定的callback
      // 可以在callback函数里检查已经重连的次数, 通过set_auto_reconnect(false)
      // 取消重连.
      if (on_reconnect_faild_cb_.get() != NULL) {
        on_reconnect_faild_cb_->Run();
      }
    }
    Poco::Thread::sleep(FLAGS_reconnect_interval);
  }
}

} // namespace
