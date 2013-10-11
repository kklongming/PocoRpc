/* 
 * File:   PocoRpcServer.h
 * Author: kk
 *
 * Created on 2013年8月28日, 下午10:26
 */

#ifndef POCORPCSERVER_H
#define	POCORPCSERVER_H

#include "base/base.h"
#include "base/fifo_queue.h"
#include "base/task_queue.h"

#include "rpclib/RpcServiceHandler.h"
#include "rpclib/RpcSocketAcceptor.h"
#include "rpclib/RpcSessionManager.h"

#include <Poco/Thread.h>
#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/SocketReactor.h>
#include <Poco/Net/SocketAcceptor.h>

namespace google {
namespace protobuf {

class Service;

}
} // namespace google

namespace Poco {

class Thread;
class FastMutex;

namespace Net {

}
}

namespace PocoRpc {

class RpcMessage;
class BaseService_Impl;

class PocoRpcServer {
 public:
  friend class BaseService_Impl;
  
  explicit PocoRpcServer(uint32 server_port, const std::string& host="0.0.0.0");
  virtual ~PocoRpcServer();

  /**
   * 注册服务. PocoRpcServer will take ownership of the Service object.
   * 将会由PocoRpcServer 负责在析构的时候delete service
   * 同一个服务只能注册一次.
   * @param service
   */
  void reg_service(::google::protobuf::Service* service);
  void run();
  void waitForTerminationRequest();
  void exit();

  void push_rpcmsg(RpcMessagePtr rpcmsg);
  void dispatch(RpcMessagePtr rpc_msg);
  RpcSessionManager*  SessionManager();
  
 private:
  
  /**
   * key: full name of the service ;
   * value: pointer of the service
   */
  typedef std::map<std::string, ::google::protobuf::Service*> ServiceMap;
  
  bool exited_;
  
  scoped_ptr<ServiceMap> service_map_;
  scoped_ptr<Poco::Net::ServerSocket> server_sock_;
  scoped_ptr<Poco::Net::SocketReactor> reactor_;
  scoped_ptr<Poco::Thread> reactor_worker_;
  scoped_ptr<RpcSocketAcceptor> acceptor_;
  
  scoped_ptr<TaskQueue> rpc_processor_;
  
  scoped_ptr<RpcSessionManager> session_mgr_;
  
  DISALLOW_COPY_AND_ASSIGN(PocoRpcServer);
};

} // namespace PocoRpc

#endif	/* POCORPCSERVER_H */

