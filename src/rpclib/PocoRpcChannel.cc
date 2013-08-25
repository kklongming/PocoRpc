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
exit_(true), connected_(false), rpc_sending_(NULL),
buf_sending_(NULL), socket_(NULL) {
  rpc_pending_.reset(new RpcControllerQueue());
  rpc_waiting_.reset(new PocoRpcControllerMap());
  mutex_waiting_response_.reset(new Poco::FastMutex());
  recv_buf_array_.reset(new BytesBufferQueue());
  mutex_recv_buf_array_.reset(new Poco::FastMutex());
  net_worker_.reset(new Poco::Thread());
  response_worker_.reset(new Poco::Thread());
  reactor_.reset(new Poco::Net::SocketReactor());
  address_.reset(new Poco::Net::SocketAddress(host, port));
}

PocoRpcChannel::~PocoRpcChannel() {
  Exit();
  STLClear(rpc_pending_.get());
  STLMapClear(rpc_waiting_.get());
  STLClear(recv_buf_array_.get());
}

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

  rpc_pending_->push(rpc_controller);
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
  return true;
}

void PocoRpcChannel::Exit() {
  CHECK(exit_ == false);
}

std::string PocoRpcChannel::DebugString() {
  std::stringstream ss;
  ss << "{" << std::endl;
  ss << "PocoRpcControllerMap : {" << std::endl;
  PocoRpcControllerMap::iterator it = rpc_waiting_->begin();
  for (; it != rpc_waiting_->end(); ++it) {
    ss << it->second->DebugString() << "," << std::endl;
    ;
  }

  ss << "}" << std::endl;
  ss << "SocketAddress : " << address_->toString() << std::endl;
  ss << "}";
  return ss.str();
}

PocoRpcController* PocoRpcChannel::NewRpcController() {
  return new PocoRpcController(this);
}

Poco::Net::StreamSocket* PocoRpcChannel::CreateSocket() {
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
    uint32 buf_size = get_rpc_msg_size(socket_.get());
    buf_recving_.reset(new BytesBuffer(buf_size));
  }
  // buf_recving_.get() != NULL
  uint32 recv_size = socket_->receiveBytes(
            reinterpret_cast<void*>(buf_recving_->pbody() + buf_recving_->get_done_size()),
            (buf_recving_->get_size() - buf_recving_->get_done_size())
            );
  buf_recving_->set_done_size(buf_recving_->get_done_size() + recv_size);
  if (buf_recving_->get_done_size() == buf_recving_->get_size()) {
    // response buf 接收 100%
    Poco::ScopedLockWithUnlock<Poco::FastMutex> lock(*mutex_recv_buf_array_);
    recv_buf_array_->push(buf_recving_.release()); // 小心, 不能使用Reset() 方法
  }
}

void PocoRpcChannel::onWritable(const Poco::AutoPtr<Poco::Net::WritableNotification>& pNf) {
  if (rpc_sending_.get() == NULL) {
    // 从 rpc_pending_ 里popup一个RpcController, 并且 IsCanceled == false
    while (true) {
      if (exit_) {
        break;
      }
      PocoRpcController* tmp_rpc = NULL;
      rpc_pending_->tryPopup(&tmp_rpc, 100);
      if (tmp_rpc != NULL) {
        if (tmp_rpc->IsCanceled() == true) {
          // rpc is canceled, so continue
          continue;
        } else { // not canceled
          rpc_sending_.reset(tmp_rpc);
          buf_sending_.reset(rpc_sending_->NewBytesBuffer());
          break;
        }
      } else { // tmp_rpc == NULL
        continue;
      }
    }
  }
  // have buf_sending_
  void* pbuf = reinterpret_cast<void*> (buf_sending_->pbody() + buf_sending_->get_done_size());
  int left_size = buf_sending_->get_size() - buf_sending_->get_done_size();
  int send_size = socket_->sendBytes(pbuf, left_size);
  if (send_size == left_size) {
    // 当前buf的所有数据发送完成 100%
    rpc_sending_.reset(NULL);
    buf_sending_.reset(NULL);

  } else {
    // 没有发送完, 等待下次发送剩余数据. 更新done_size
    buf_sending_->set_done_size(buf_sending_->get_done_size() + send_size);
  }
}

void PocoRpcChannel::onShutdown(const Poco::AutoPtr<Poco::Net::ShutdownNotification>& pNf) {

}

void PocoRpcChannel::onError(const Poco::AutoPtr<Poco::Net::ErrorNotification>& pNf) {
  reactor_->stop();
  unreg_reactor_handler(socket_.get());
  socket_.reset(NULL);
}

} // namespace
