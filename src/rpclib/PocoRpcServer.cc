/* 
 * File:   PocoRpcServer.cc
 * Author: kk
 * 
 * Created on 2013年8月28日, 下午10:26
 */

#include "rpclib/PocoRpcServer.h"
#include "rpclib/BaseService_Impl.h"
#include "rpclib/PocoRpcSocketReactor.h"

#include "base/runable.h"
#include "base/daemon.h"
#include "rpc_def/base_service.pb.h"

#include <google/protobuf/service.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include <Poco/Net/SocketAddress.h>

DEFINE_int32(rpc_session_timeout, 1800000, "rpc session timeout in milliseconds");
DEFINE_int32(rpc_check_interval, 300000, "rpc session timeout check interval in milliseconds");
DEFINE_int32(rpc_worker_count_, 1, "rpc worker count");

namespace PocoRpc {

PocoRpcServer::PocoRpcServer(uint32 server_port, const std::string& host) :
exited_(false), acceptor_(NULL) {
  service_map_.reset(new ServiceMap());
  server_sock_.reset(new Poco::Net::ServerSocket(Poco::Net::SocketAddress(host, (unsigned short) server_port)));
  reactor_.reset(new PocoRpcSocketReactor());
  reactor_worker_.reset(new Poco::Thread());
  rpc_processor_.reset(new TaskQueue(FLAGS_rpc_worker_count_));
  session_mgr_.reset(new RpcSessionManager(FLAGS_rpc_session_timeout,
          FLAGS_rpc_check_interval));

  ::google::protobuf::Service* base_svc = new BaseService_Impl(this);
  reg_service(base_svc);
}

PocoRpcServer::~PocoRpcServer() {
  if (not exited_) {
    exit();
  }
  STLMapClear(service_map_.get());
}

void PocoRpcServer::reg_service(::google::protobuf::Service* service) {
  std::string service_full_name = service->GetDescriptor()->full_name();
  ServiceMap::iterator it = service_map_->find(service_full_name);
  CHECK(it == service_map_->end()) << "service has been regist before.";
  service_map_->insert(std::pair<std::string, ::google::protobuf::Service*>(
          service_full_name, service));
}

void PocoRpcServer::run() {
  acceptor_.reset(new RpcSocketAcceptor(this, *server_sock_, *reactor_));
  rpc_processor_->Start();
  reactor_worker_->start(*reactor_);
}

void PocoRpcServer::waitForTerminationRequest() {
  ::waitForTerminationRequest();
}

void PocoRpcServer::exit() {
  reactor_->stop();
  reactor_worker_->join();
  rpc_processor_->StopImmediately();
  rpc_processor_->ClearTasks();
  server_sock_->close();
  exited_ = true;
}

void PocoRpcServer::push_rpcmsg(RpcMessagePtr rpcmsg) {
  Poco::Runnable* task = Poco::NewPermanentCallback(this, &PocoRpcServer::dispatch, rpcmsg);
  rpc_processor_->AddTask(task);
}

void PocoRpcServer::dispatch(RpcMessagePtr rpc_msg) {
  const ::google::protobuf::DescriptorPool* descriptor_pool = ::google::protobuf::DescriptorPool::generated_pool();
  const ::google::protobuf::MethodDescriptor* method_desc = descriptor_pool->FindMethodByName(rpc_msg->method_full_name());
  CHECK(method_desc != NULL) << "Can not find MethodDescriptor by name: " << rpc_msg->method_full_name();
  const ::google::protobuf::ServiceDescriptor* service_desc = method_desc->service();

  std::string method_full_name = service_desc->full_name();
  ServiceMap::iterator it = service_map_->find(method_full_name);
  if (it == service_map_->end()) {
    LOG(ERROR) << "Can not find this rpc method: " << method_full_name;
    return;
  }

  ::google::protobuf::Service* service = it->second;
  const ::google::protobuf::Descriptor* req_desc = method_desc->input_type();
  const ::google::protobuf::Descriptor* res_desc = method_desc->output_type();
  ::google::protobuf::MessageFactory* msg_factory = ::google::protobuf::MessageFactory::generated_factory();


  scoped_ptr< ::google::protobuf::Message > req_msg(msg_factory->GetPrototype(req_desc)->New());
  CHECK(req_msg.get() != NULL) << "Faild to create Message by Descriptor: " << req_desc->full_name();

  scoped_ptr< ::google::protobuf::Message > res_msg(msg_factory->GetPrototype(res_desc)->New());
  CHECK(res_msg.get() != NULL) << "Faild to create Message by Descriptor: " << res_desc->full_name();


  if (not req_msg->ParseFromString(rpc_msg->message_body())) {
    LOG(ERROR) << "Parse request faild. rpc method: " << rpc_msg->method_full_name();
    return;
  }

  service->CallMethod(method_desc, NULL, req_msg.get(), res_msg.get(), NULL);

  // todo : 改成check and LOG(ERROR)
  CHECK(res_msg->IsInitialized()) << "Not all required fields have values set. rpc method: " << rpc_msg->method_full_name();

  RpcMessagePtr response_rpcmsg(new RpcMessage());
  response_rpcmsg->set_id(rpc_msg->id());
  response_rpcmsg->set_client_uuid(rpc_msg->client_uuid());
  response_rpcmsg->set_method_full_name(rpc_msg->method_full_name());
  response_rpcmsg->set_message_body(res_msg->SerializeAsString());

  RpcSessionPtr session = session_mgr_->FindOrCreate(rpc_msg->client_uuid());
  session->push(response_rpcmsg);
}

RpcSessionManager* PocoRpcServer::SessionManager() {
  return session_mgr_.get();
}

} // namespace PocoRpc

