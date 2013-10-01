/* 
 * File:   BaseService_Impl.h
 * Author: kk
 *
 * Created on 2013年9月20日, 上午11:40
 */

#ifndef BASESERVICE_IMPL_H
#define	BASESERVICE_IMPL_H

#include "base/base.h"
#include "rpc_def/base_service.pb.h"
#include "rpclib/PocoRpcServer.h"

namespace PocoRpc {

class BaseService_Impl : public BaseService {
 public:
  explicit BaseService_Impl(PocoRpcServer* RpcServer);
  virtual ~BaseService_Impl();

  virtual void Ping(::google::protobuf::RpcController* controller,
          const ::PocoRpc::PingReq* request,
          ::PocoRpc::PingReply* response,
          ::google::protobuf::Closure* done);
  virtual void GetServiceList(::google::protobuf::RpcController* controller,
          const ::PocoRpc::GetServiceListReq* request,
          ::PocoRpc::GetServiceListReply* response,
          ::google::protobuf::Closure* done);

 private:
  PocoRpcServer* server_;

  DISALLOW_COPY_AND_ASSIGN(BaseService_Impl);
};

}

#endif	/* BASESERVICE_IMPL_H */

