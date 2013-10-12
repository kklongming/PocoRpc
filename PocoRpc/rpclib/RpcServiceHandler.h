/* 
 * File:   RpcServiceHandler.h
 * Author: kk
 *
 * Created on 2013年8月29日, 下午3:23
 */

#ifndef RPCSERVICEHANDLER_H
#define	RPCSERVICEHANDLER_H

#include "PocoRpc/base/base.h"
#include "PocoRpc/rpclib/BytesBuffer.h"
#include "PocoRpc/rpclib/RpcSession.h"

#include <Poco/AutoPtr.h>
#include <Poco/Net/StreamSocket.h>


namespace Poco {

class Thread;
class FastMutex;

namespace Net {

class SocketReactor;
class ReadableNotification;
class WritableNotification;
class ErrorNotification;
class ShutdownNotification;

}
}

namespace PocoRpc {

class PocoRpcServer;

class RpcServiceHandler {
 public:
  RpcServiceHandler(Poco::Net::StreamSocket& socket,
          Poco::Net::SocketReactor& reactor);
  
  RpcServiceHandler(PocoRpcServer* rpc_server,
          Poco::Net::StreamSocket& socket,
          Poco::Net::SocketReactor& reactor);

  virtual ~RpcServiceHandler();
  
 private:
  std::string client_uuid_;
  PocoRpcServer *rpc_server_;
  Poco::Net::StreamSocket socket_;
  Poco::Net::SocketReactor& reactor_;
  
  // 正在接收中的 buf
  scoped_ptr<BytesBuffer> recving_buf_;
  
  // 正在发送中的 buf
  scoped_ptr<BytesBuffer> sending_buf_;
  
  bool onWritable_ready_;
  scoped_ptr<Poco::FastMutex> mutex_onWritable_ready_;
  void reg_onWritable();
  void unreg_onWritable();

  void reg_handler();
  void unreg_handler();
  void onSocketError();

  void onReadable(const Poco::AutoPtr<Poco::Net::ReadableNotification>& pNf);
  void onWritable(const Poco::AutoPtr<Poco::Net::WritableNotification>& pNf);
  void onShutdown(const Poco::AutoPtr<Poco::Net::ShutdownNotification>& pNf);
  void onError(const Poco::AutoPtr<Poco::Net::ErrorNotification>& pNf);
  
  void on_pushed_cb(RpcSession::RpcMsgQueue* queue);
  void on_popuped_cb(RpcSession::RpcMsgQueue* queue);

  DISALLOW_COPY_AND_ASSIGN(RpcServiceHandler);
};

} // namespace PocoRpc

#endif	/* RPCSERVICEHANDLER_H */

