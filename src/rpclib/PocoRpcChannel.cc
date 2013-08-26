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

namespace PocoRpc {

PocoRpcChannel::PocoRpcChannel(const std::string& host, uint16 port) :
exit_(true), connected_(false), rpc_sending_(NULL), reacotr_running_(false),
buf_sending_(NULL), socket_(NULL) {
  rpc_pending_.reset(new RpcControllerQueue());
  rpc_waiting_.reset(new PocoRpcControllerMap());
  mutex_waiting_response_.reset(new Poco::FastMutex());
  recv_buf_array_.reset(new BytesBufferQueue());
  mutex_recv_buf_array_.reset(new Poco::FastMutex());
  net_worker_.reset(new Poco::Thread());
  response_worker_.reset(new Poco::Thread());
  ra_response_.reset(Poco::NewPermanentCallback(this, &PocoRpcChannel::process_response));
  reactor_.reset(new Poco::Net::SocketReactor());
  address_.reset(new Poco::Net::SocketAddress(host, port));
}

PocoRpcChannel::~PocoRpcChannel() {
  Exit();

  rpc_pending_->clear();
  rpc_waiting_->clear();
  STLClear(recv_buf_array_.get());
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
    connected_ = false;
    return false;
  }
  // successed cennected with rpc server
  reg_reactor_handler(socket_.get());
  connected_ = true;
  net_worker_->start(*reactor_);
  reacotr_running_ = true;

  response_worker_->start(*ra_response_);

  return true;
}

void PocoRpcChannel::Exit() {
  CHECK(exit_ == false);
  exit_ = true;
  reactor_->stop();
  unreg_reactor_handler(socket_.get());
  net_worker_->join();
  reacotr_running_ = false;

  response_worker_->join();

  cancel_waiting_response_rpc("Normal exit");
}

/**
 * 根据rpc_id, 将PocoRpcChannel 内部几个队列中, id相同的RpcController cancel掉
 * @param rpc_id
 * @param reason: cancel的原因
 */
void PocoRpcChannel::CancelRpc(uint64 rpc_id, const std::string& reason) {
  AutoPocoRpcControllerPtr rpc_ctrl(NULL);

  rpc_pending_->lock();
  RpcControllerQueue::iterator it_pending = rpc_pending_->begin();

  for (; it_pending != rpc_pending_->end(); ++it_pending) {
    if ((*it_pending)->id() == rpc_id) {
      rpc_ctrl = *it_pending;
      rpc_pending_->erase(it_pending);
      break;
    }
  }
  rpc_pending_->unlock();

  if (!rpc_ctrl.isNull()) {
    rpc_ctrl->SetFailed(reason);
    rpc_ctrl->StartCancel();
    return;
  }

  /// 在 rpc_pending_ 队列里没有找到, 则继续在 rpc_waiting_ 队列里找
  {
    Poco::ScopedLockWithUnlock<Poco::FastMutex> lock(*mutex_waiting_response_);
    PocoRpcControllerMap::iterator it_waiting = rpc_waiting_->find(rpc_id);
    if (it_waiting != rpc_waiting_->end()) { // 找到对应的
      rpc_ctrl = it_waiting->second;
      rpc_waiting_->erase(it_waiting);
    }
  }

  if (!rpc_ctrl.isNull()) {
    rpc_ctrl->SetFailed(reason);
    rpc_ctrl->StartCancel();
    return;
  }
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

Poco::Net::StreamSocket* PocoRpcChannel::CreateSocket() {
  // @todo 增加flags, 提供参数设置socket的选项
  Poco::Net::StreamSocket* sock = new Poco::Net::StreamSocket(*address_);
  sock->setBlocking(false);
  sock->setKeepAlive(true);
  sock->setNoDelay(true);
  sock->setReceiveTimeout(Poco::Timespan(5, 0));
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
      // 先读4个字节头部, 得到body的size
      uint32 buf;
      int recv_size = socket_->receiveBytes(reinterpret_cast<void*> (&buf), 4);
      if (recv_size == 0) {
        // 0 means socket was shutdown by server side.
        on_socket_error();
        return;
      }
 
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
    if (recv_size == 0) {
      // 0 means socket was shutdown by server side.
      on_socket_error();
      return;
    }

    buf_recving_->set_done_size(buf_recving_->get_done_size() + recv_size);
    if (buf_recving_->get_done_size() == buf_recving_->get_size()) {
      // response buf 接收 100%
      Poco::ScopedLockWithUnlock<Poco::FastMutex> lock(*mutex_recv_buf_array_);
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
      rpc_pending_->tryPopup(&tmp_rpc, 100);
      if (not tmp_rpc.isNull()) {
        if (tmp_rpc->IsCanceled()) {
          // rpc is canceled, so ignore it and continue to get next one
          continue;
        } else { // not canceled
          rpc_sending_.assign(tmp_rpc);
          buf_sending_.reset(rpc_sending_->NewBytesBuffer());
          break;
        }
      } else { // tmp_rpc is NULL
        continue;
      }
    }
  }
  // buf_sending_ ready
  void* pbuf = reinterpret_cast<void*> (buf_sending_->pbody() + buf_sending_->get_done_size());
  int left_size = buf_sending_->get_size() - buf_sending_->get_done_size();
  int send_size = socket_->sendBytes(pbuf, left_size);
  if (send_size == -1) {
    LOG(ERROR) << "socket send error: " << errno;
    on_socket_error();
    return;
  }

  if (send_size == left_size) {
    // 当前buf的所有数据发送完成 100%, rpc 移动到rpc_waiting_ 队列里.
    Poco::ScopedLockWithUnlock<Poco::FastMutex> lock(*mutex_waiting_response_);
    rpc_waiting_->insert(std::pair<uint64, AutoPocoRpcControllerPtr>(
            rpc_sending_->id(),
            rpc_sending_));
    rpc_sending_.assign(NULL);
    buf_sending_.reset(NULL);
  } else {
    // 没有发送完, 等待下次发送剩余数据. 更新done_size
    buf_sending_->set_done_size(buf_sending_->get_done_size() + send_size);
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
    Poco::ScopedLockWithUnlock<Poco::FastMutex> lock(*mutex_recv_buf_array_);
    BytesBuffer* recved_buf = NULL;
    recv_buf_array_->tryPopup(&recved_buf, 200);
    if (recved_buf != NULL) {
      scoped_ptr<RpcMessage> rpc_msg(new RpcMessage());
      CHECK(rpc_msg->ParseFromArray(recved_buf->pbody(),
              recved_buf->get_size())) << "RpcMessage 反序列化出错";
      AutoPocoRpcControllerPtr rpc_ctrl = NULL;
      {
        Poco::ScopedLockWithUnlock<Poco::FastMutex> lock_rpc_waiting_(*mutex_waiting_response_);
        PocoRpcControllerMap::iterator it_rpc = rpc_waiting_->find(rpc_msg->id());
        if (it_rpc != rpc_waiting_->end()) {
          rpc_ctrl = it_rpc->second;
          rpc_waiting_->erase(it_rpc);
        }
      }
      if (!rpc_ctrl.isNull()) {
        rpc_ctrl->signal_rpc_over();
      }
    }
  }
}

/**
 * Cancel 掉 rpc_waiting_ 里所有已经发送Request, 等待response的rpc.
 * 
 * @param reason : cancel的原因
 */
void PocoRpcChannel::cancel_waiting_response_rpc(const std::string& reason) {
  Poco::ScopedLockWithUnlock<Poco::FastMutex> lock(*mutex_waiting_response_);
  PocoRpcControllerMap::iterator it = rpc_waiting_->begin();
  for (; it != rpc_waiting_->end(); ++it) {
    AutoPocoRpcControllerPtr rpc_ctrl = it->second;
    rpc_ctrl->SetFailed(reason);
    rpc_ctrl->StartCancel();
  }
}

/// on_socket_error() 是在reactor的线程上被调用的, 所以不会有并发的问题.
/// 但是它可能会被多次调用. 所以要进行一些检查, 要采取防御性编码

void PocoRpcChannel::on_socket_error() {
  if (not reacotr_running_) {
    // 1. 停止 reactor 线程
    reactor_->stop();
    net_worker_->join();
    reacotr_running_ = false;
  }

  // 2. 关闭socket
  if (socket_.get() != NULL) {
    socket_->shutdown();
    socket_.reset(NULL);
  }

  // 3. cancel 掉所有已经发送Request, 等待Response的Rpc. 因为socket断掉后,
  //    这些rpc已经没有机会再收到Response了.
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

}

} // namespace
