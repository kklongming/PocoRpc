/* 
 * File:   RpcServiceHandler.cc
 * Author: kk
 * 
 * Created on 2013年8月29日, 下午3:23
 */

#include "PocoRpc/base/runable.h"
#include "PocoRpc/rpclib/RpcServiceHandler.h"
#include "PocoRpc/rpclib/PocoRpcServer.h"
#include "PocoRpc/rpc_proto/poco_rpc.pb.h"
#include <Poco/Net/SocketReactor.h>
#include <Poco/Net/SocketNotification.h>
#include <Poco/NObserver.h>
#include <Poco/Mutex.h>
#include <Poco/ScopedLock.h>

namespace PocoRpc {

RpcServiceHandler::RpcServiceHandler(Poco::Net::StreamSocket& socket,
        Poco::Net::SocketReactor& reactor) : rpc_server_(NULL),
socket_(socket), client_uuid_(""),
reactor_(reactor), recving_buf_(NULL), sending_buf_(NULL),
onWritable_ready_(false) {
  mutex_onWritable_ready_.reset(new Poco::FastMutex());
  reg_handler();
  CHECK(false) << "Should not call this constuctor.";
}

RpcServiceHandler::RpcServiceHandler(PocoRpcServer *rpc_server,
        Poco::Net::StreamSocket& socket,
        Poco::Net::SocketReactor& reactor) : rpc_server_(rpc_server),
socket_(socket), client_uuid_(""),
reactor_(reactor), recving_buf_(NULL), sending_buf_(NULL),
onWritable_ready_(false) {
  mutex_onWritable_ready_.reset(new Poco::FastMutex());
  reg_handler();
  LOG(INFO) << "Create RpcServiceHandler::RpcServiceHandler";
}

RpcServiceHandler::~RpcServiceHandler() {
  if (not client_uuid_.empty()) {
    RpcSessionPtr session = rpc_server_->SessionManager()->FindOrCreate(client_uuid_);
    session->clear_on_popuped_cb();
    session->clear_on_pushed_cb();
    session->release_service_handler(this);
  }
  unreg_handler();
  LOG(INFO) << "unreg_handler();";
  socket_.close();
  LOG(INFO) << "Destory RpcServiceHandler::RpcServiceHandler";
}

void RpcServiceHandler::reg_onWritable() {
  Poco::ScopedLock<Poco::FastMutex> lock(*mutex_onWritable_ready_);
  if (onWritable_ready_) {
    return;
  }
  reactor_.addEventHandler(socket_,
          Poco::NObserver<RpcServiceHandler, Poco::Net::WritableNotification>(*this,
          &RpcServiceHandler::onWritable));
  onWritable_ready_ = true;
}

void RpcServiceHandler::unreg_onWritable() {
  Poco::ScopedLock<Poco::FastMutex> lock(*mutex_onWritable_ready_);
  if (not onWritable_ready_) {
    return;
  }
  reactor_.removeEventHandler(socket_,
          Poco::NObserver<RpcServiceHandler, Poco::Net::WritableNotification>(*this,
          &RpcServiceHandler::onWritable));
  onWritable_ready_ = false;
}

void RpcServiceHandler::reg_handler() {
  reactor_.addEventHandler(socket_,
          Poco::NObserver<RpcServiceHandler, Poco::Net::ReadableNotification>(*this,
          &RpcServiceHandler::onReadable));
  reactor_.addEventHandler(socket_,
          Poco::NObserver<RpcServiceHandler, Poco::Net::ShutdownNotification>(*this,
          &RpcServiceHandler::onShutdown));
  reactor_.addEventHandler(socket_,
          Poco::NObserver<RpcServiceHandler, Poco::Net::ErrorNotification>(*this,
          &RpcServiceHandler::onError));
  reg_onWritable();
  LOG(INFO) << "RpcServiceHandler::reg_handler";
}

void RpcServiceHandler::unreg_handler() {
  reactor_.removeEventHandler(socket_,
          Poco::NObserver<RpcServiceHandler, Poco::Net::ReadableNotification>(*this,
          &RpcServiceHandler::onReadable));
  reactor_.removeEventHandler(socket_,
          Poco::NObserver<RpcServiceHandler, Poco::Net::ShutdownNotification>(*this,
          &RpcServiceHandler::onShutdown));
  reactor_.removeEventHandler(socket_,
          Poco::NObserver<RpcServiceHandler, Poco::Net::ErrorNotification>(*this,
          &RpcServiceHandler::onError));
  unreg_onWritable();
}

void RpcServiceHandler::onSocketError() {
  LOG(ERROR) << "********** onSocketError **********";
  delete this;
}

void RpcServiceHandler::onReadable(const Poco::AutoPtr<Poco::Net::ReadableNotification>& pNf) {
//  LOG(INFO) << "socket is readable";
  if (socket_.available() == 0) {
    LOG(INFO) << "socket is readable status, but it's available data length == 0, so close this socket.";
    onSocketError();
    return;
  }
  // 开始接收rpc request
  if (recving_buf_.get() == NULL) {
    try {
      // 检查 socket buffer 里是否至少够4 Bytes (即length header)
      if (socket_.available() < 4) {
        // 不够4 Bytes, 则啥都不做,等下次在处理
        return;
      }

      // 到这里, 肯定是大于 4 Bytes, 先读4个Bytes 得到body的size
      char buf[4] = {0, 0, 0, 0};
      int recv_size = socket_.receiveBytes(reinterpret_cast<void*> (&buf), 4);
      // 这里需要检查一下返回值, 判断recv的过程中是否出错
      if (recv_size == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) {
          LOG(WARNING) << "socket is EAGAIN or EWOULDBLOCK or EINTR";
          return;
        } else {
          LOG(ERROR) << "socket recv ERROR: " << errno;
          onSocketError();
          return;
        }
      }

      if (recv_size == 0) {
        // 0 means socket was shutdown by client side.
        LOG(ERROR) << "socket recv == 0 ";
        onSocketError();
        return;
      }

      if (recv_size != 4) {
        LOG(ERROR) << "recv_size != 4";
        onSocketError();
        return;
      }

      uint32* puint32 = (uint32*) buf;
      uint32 buf_size = ntohl(*puint32);
      recving_buf_.reset(new BytesBuffer(buf_size));
      recving_buf_->set_done_size(4);
    } catch (Poco::Exception ex) {
      LOG(ERROR) << ex.message();
      onSocketError();
      return;
    }
  }

  // 已经正确读出body的size了, 并且创建了 recving_buf_
  try {
    uint32 recv_size = socket_.receiveBytes(
            reinterpret_cast<void*> (recving_buf_->phead() + recving_buf_->get_done_size()),
            (recving_buf_->get_total_size() - recving_buf_->get_done_size())
            );
    // 这里需要检查一下返回值, 判断recv的过程中是否出错
    if (recv_size == -1) {
      if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) {
        LOG(WARNING) << "socket is EAGAIN or EWOULDBLOCK or EINTR";
        return;
      } else {
        LOG(ERROR) << "socket recv ERROR: " << errno;
        onSocketError();
        return;
      }
    }

    if (recv_size == 0) {
      // 0 means socket was shutdown by client side.
      LOG(ERROR) << "try read " << (recving_buf_->get_total_size() - recving_buf_->get_done_size()) << " bytes and socket recv == 0 ";
      onSocketError();
      return;
    }

    recving_buf_->set_done_size(recving_buf_->get_done_size() + recv_size);
    if (recving_buf_->get_done_size() == recving_buf_->get_total_size()) {
      // buf 接收 100%
      scoped_ptr<RpcMessage> rpc_msg(new RpcMessage());
      if (not rpc_msg->ParseFromArray(recving_buf_->pbody(),
              recving_buf_->get_body_size())) {
        // 反序列化出错的原因, 可能是非法客户端发送错误的数据过来
        // socket 通讯出了问题. 不管是那一种, 我们都采取关闭socket的处理方法
        LOG(ERROR) << "RpcMessage 反序列化出错";
        onSocketError();
        return;
      }

      // 正常情况, 接收100% 并且反序列化成功
      if (client_uuid_.empty()) {
        // RpcServiceHandler 创建后, socket 接收到的第一个Rpc请求, 根据这个rpc
        // 请求设置client_uuid_, 并注册on_pushed_cb, on_popuped_cb
        client_uuid_ = rpc_msg->client_uuid();
        RpcSessionPtr session = rpc_server_->SessionManager()->FindOrCreate(client_uuid_);
        session->reset_service_handler(this);  
        /// reset_service_handler(this) 必须在session->reg_on_pushed_cb(on_pushed_call) 和 
        /// session->reg_on_popuped_cb(on_popuped_call); 之前
        Poco::Runnable* on_pushed_call = Poco::NewPermanentCallback(this,
                &RpcServiceHandler::on_pushed_cb, session->pending_response_.get());
        Poco::Runnable* on_popuped_call = Poco::NewPermanentCallback(this,
                &RpcServiceHandler::on_popuped_cb, session->pending_response_.get());
        session->clear_on_pushed_cb();
        session->clear_on_popuped_cb();
        session->reg_on_pushed_cb(on_pushed_call);
        session->reg_on_popuped_cb(on_popuped_call);
        LOG(INFO) << "==>" << "get client_uuid_=" << client_uuid_;
      }
      CHECK(client_uuid_ == rpc_msg->client_uuid());
      RpcMessagePtr rpc_msg_ptr(rpc_msg.release());
      // push rpcmsg to rpcserver 
      rpc_server_->push_rpcmsg(rpc_msg_ptr);

      recving_buf_.release();
    }

  } catch (Poco::Exception ex) {
    LOG(ERROR) << ex.message();
    onSocketError();
    return;
  }

}

void RpcServiceHandler::onWritable(const Poco::AutoPtr<Poco::Net::WritableNotification>& pNf) {
//  LOG(INFO) << "socket is writeable";
  if (client_uuid_.empty()) {
    return;
  }
  /// 根据uuid找到或者新建一个Session
  RpcSessionPtr session = rpc_server_->SessionManager()->FindOrCreate(client_uuid_);

  if (sending_buf_.get() == NULL) {
    /// 没有正在发送的buf, 则从session里pending_response_ 队列里取一个RpcMessagePtr
    RpcMessagePtr p_rpcmsg;
    session->tryPopup(&p_rpcmsg, 0);
    if (p_rpcmsg.get() == NULL) {
      /// 没有需要发送的response, 则退出等待下一次onWrite 事件发生
      return;
    }
    // 得到一个需要发送的Response RpcMsg, 根据其创建一个Buffer对象用于发送
    sending_buf_.reset(new BytesBuffer(p_rpcmsg->ByteSize()));
    p_rpcmsg->SerializePartialToArray(reinterpret_cast<void*> (sending_buf_->pbody()),
            sending_buf_->get_body_size());
  }
  CHECK(sending_buf_.get() != NULL);

  try {
    void* pbuf = reinterpret_cast<void*> (sending_buf_->phead() + sending_buf_->get_done_size());
    int send_size = socket_.sendBytes(pbuf, sending_buf_->get_total_size() - sending_buf_->get_done_size());
    if (send_size == -1) {
      if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) {
        LOG(WARNING) << "socket is EAGAIN or EWOULDBLOCK or EINTR";
        return;
      } else {
        LOG(ERROR) << "socket send ERROR: " << errno;
        onSocketError();
        return;
      }
    }

    sending_buf_->set_done_size(sending_buf_->get_done_size() + send_size);
    if (sending_buf_->get_done_size() == sending_buf_->get_total_size()) {
      /// 发送完成 100%
      sending_buf_.release();
      session->ReleaseSendingRpcmsg();
    }

  } catch (Poco::Exception ex) {
    LOG(ERROR) << ex.message();
    onSocketError();
    return;
  }
}

void RpcServiceHandler::onShutdown(const Poco::AutoPtr<Poco::Net::ShutdownNotification>& pNf) {
  LOG(INFO) << "callback onShutdown()";
  delete this;
}

void RpcServiceHandler::onError(const Poco::AutoPtr<Poco::Net::ErrorNotification>& pNf) {
  LOG(INFO) << "callback onError()";
  onSocketError();
}

void RpcServiceHandler::on_pushed_cb(RpcSession::RpcMsgQueue* queue) {
  if (queue->size() == 1) {
    reg_onWritable();
    LOG(INFO) << "regist socket on_writeable";
  }
}

void RpcServiceHandler::on_popuped_cb(RpcSession::RpcMsgQueue* queue) {
  if (queue->size() == 0) {
    unreg_onWritable();
    LOG(INFO) << "UNREGIST socket on_writeable";
  }
}

} // namespace PocoRpc

