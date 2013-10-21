/* 
 * File:   PocoRpcSocketReactor.cc
 * Author: kk
 * 
 * Created on 2013年10月2日, 下午4:25
 */

#include "PocoRpc/rpclib/PocoRpcSocketReactor.h"
#include <Poco/Thread.h>

DEFINE_int32(reactor_idle_time, 25, "sleep time in ms when there are no sockets for the SocketReactor");

namespace PocoRpc {

PocoRpcSocketReactor::PocoRpcSocketReactor() {
}

PocoRpcSocketReactor::~PocoRpcSocketReactor() {
}

void PocoRpcSocketReactor::onIdle() {
  Poco::Net::SocketReactor::onIdle();
  Poco::Thread::sleep(FLAGS_reactor_idle_time);
}

} // namespace PocoRpc {

