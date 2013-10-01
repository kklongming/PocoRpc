/* 
 * File:   BaseService_Impl.cc
 * Author: kk
 * 
 * Created on 2013年9月20日, 上午11:40
 */

#include "rpclib/PocoRpcError.h"
#include "BaseService_Impl.h"


namespace PocoRpc {

BaseService_Impl::BaseService_Impl(PocoRpcServer* RpcServer) : server_(RpcServer) {
}

BaseService_Impl::~BaseService_Impl() {
}

void BaseService_Impl::Ping(::google::protobuf::RpcController* controller,
        const ::PocoRpc::PingReq* request,
        ::PocoRpc::PingReply* response,
        ::google::protobuf::Closure* done) {
  LOG(INFO) << "recive Ping Request";
  response->set_status(E_OK);
}

void BaseService_Impl::GetServiceList(::google::protobuf::RpcController* controller,
        const ::PocoRpc::GetServiceListReq* request,
        ::PocoRpc::GetServiceListReply* response,
        ::google::protobuf::Closure* done) {
  LOG(INFO) << "recive GetServiceList Request";
  PocoRpcServer::ServiceMap::iterator it = server_->service_map_->begin();
  for (; it != server_->service_map_->end(); ++it) {
    std::string* v = response->mutable_service_list()->Add();
    *v = it->first;
  }
  response->set_status(E_OK);
}

} // namespace PocoRpc
