/* 
 * File:   RpcSocketAcceptor.h
 * Author: kk
 *
 * Created on 2013年8月31日, 下午3:17
 */

#ifndef RPCSOCKETACCEPTOR_H
#define	RPCSOCKETACCEPTOR_H

#include "PocoRpc/base/base.h"
#include "PocoRpc/rpclib/RpcServiceHandler.h"
#include <Poco/Net/SocketReactor.h>
#include <Poco/Net/SocketAcceptor.h>

using Poco::Net::SocketAcceptor;

namespace Poco {

class Thread;
class FastMutex;

namespace Net {

class StreamSocket;
class ServerSocket;
class SocketReactor;

}
}

using Poco::Net::StreamSocket;

namespace PocoRpc {

class PocoRpcServer;

class RpcSocketAcceptor : public SocketAcceptor<RpcServiceHandler> {
 public:
  RpcSocketAcceptor(PocoRpcServer *rpc_server, Poco::Net::ServerSocket& socket,
          Poco::Net::SocketReactor& reactor);

  virtual ~RpcSocketAcceptor();

 
  virtual RpcServiceHandler* createServiceHandler(StreamSocket& socket);

 private:
  PocoRpcServer *rpc_server_;

  DISALLOW_COPY_AND_ASSIGN(RpcSocketAcceptor);

};

} // namespace PocoRpc

#endif	/* RPCSOCKETACCEPTOR_H */

