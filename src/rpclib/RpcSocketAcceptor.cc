/* 
 * File:   RpcSocketAcceptor.cc
 * Author: kk
 * 
 * Created on 2013年8月31日, 下午3:17
 */

#include "rpclib/RpcSocketAcceptor.h"

#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/StreamSocket.h>
#include <Poco/Net/SocketReactor.h>

namespace PocoRpc {

RpcSocketAcceptor::RpcSocketAcceptor(PocoRpcServer *rpc_server, 
        Poco::Net::ServerSocket& socket,
        Poco::Net::SocketReactor& reactor) : rpc_server_(rpc_server), 
        SocketAcceptor<RpcServiceHandler>(socket, reactor) {
}

RpcSocketAcceptor::~RpcSocketAcceptor() {
  
}

RpcServiceHandler* RpcSocketAcceptor::createServiceHandler(StreamSocket& socket) {
  LOG(INFO) << "New connection comming...";
  Poco::Net::SocketReactor *pReactor = reactor();
  CHECK(pReactor != NULL);
  return new RpcServiceHandler(rpc_server_, socket, *pReactor);
}

} // namespace PocoRpc