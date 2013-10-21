/* 
 * File:   RpcSocketAcceptor.cc
 * Author: kk
 * 
 * Created on 2013年8月31日, 下午3:17
 */

#include "PocoRpc/rpclib/RpcSocketAcceptor.h"

#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/StreamSocket.h>
#include <Poco/Net/SocketReactor.h>

namespace PocoRpc {

RpcSocketAcceptor::RpcSocketAcceptor(PocoRpcServer *rpc_server, 
        Poco::Net::ServerSocket& socket,
        Poco::Net::SocketReactor& read_reactor,
        Poco::Net::SocketReactor& write_reactor) : rpc_server_(rpc_server), 
        SocketAcceptor<RpcServiceHandler>(socket, read_reactor) {
  p_write_reactor_ = &write_reactor;
}

RpcSocketAcceptor::~RpcSocketAcceptor() {
  
}

RpcServiceHandler* RpcSocketAcceptor::createServiceHandler(StreamSocket& socket) {
  LOG(INFO) << "New connection comming...";
  Poco::Net::SocketReactor *p_read_reactor = reactor();
  CHECK(p_read_reactor != NULL);
  return new RpcServiceHandler(rpc_server_, socket, *p_read_reactor, *p_write_reactor_);
}

} // namespace PocoRpc