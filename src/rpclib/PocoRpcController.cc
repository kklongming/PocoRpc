/* 
 * File:   PocoRpcController.cc
 * Author: kk
 * 
 * Created on 2013年8月11日, 下午10:21
 */

#include "rpclib/PocoRpcController.h"

#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>
#include <Poco/ScopedLock.h>

namespace PocoRpc {

PocoRpcController::PocoRpcController(PocoRpcChannel* rpc_ch) :
    poco_rpc_ch_(rpc_ch),
    successed_(false),
    error_text_("Not called"),
    is_canceled_(false),
    on_cancel_callback_(NULL),
    method_desc_(NULL),
    request_(NULL),
    response_(NULL),
    on_done_callback_(NULL) {
  id_ = genernate_rpc_id();
  rpc_condt_mutex_.reset(new Poco::FastMutex());
  rpc_condt_.reset(new Poco::Condition());
}

PocoRpcController::~PocoRpcController() {
}

void PocoRpcController::Reset() {

}

bool PocoRpcController::Failed() const {
  return !successed_;
}

string PocoRpcController::ErrorText() {
  return error_text_;
}

void PocoRpcController::StartCancel() {
  is_canceled_ = true;
  if (on_cancel_callback_ != NULL) {
    on_cancel_callback_->Run();
  }
  // to do: 在PocoRpcChannel 内处理任务队列
}

void PocoRpcController::SetFailed(const string& reason) {
  error_text_ = reason;
}

bool PocoRpcController::IsCanceled() {
  return is_canceled_;
}

void PocoRpcController::NotifyOnCancel(Closure* callback) {
  on_cancel_callback_ = callback;
}

void PocoRpcController::set_method_desc(const MethodDescriptor* method_desc) {
  method_desc_ = method_desc;
}

void PocoRpcController::set_request(const Message* request) {
  request_ = request;
}

void PocoRpcController::set_response(Message* response) {
  response_ = response;
}

void PocoRpcController::set_on_done_callback(Closure* callback) {
  on_done_callback_ = callback;
}

void PocoRpcController::wait() {
  Poco::ScopedLockWithUnlock<Poco::FastMutex> lock(*rpc_condt_mutex_);
  rpc_condt_->wait(*rpc_condt_mutex_);
}

bool PocoRpcController::tryWait(long milliseconds) {
  Poco::ScopedLockWithUnlock<Poco::FastMutex> lock(*rpc_condt_mutex_);
  return rpc_condt_->tryWait(*rpc_condt_mutex_, milliseconds);
}

void PocoRpcController::signal_rpc_over() {
  rpc_condt_->broadcast();
}

string PocoRpcController::DebugString() {
  std::stringstream ss;
  ss << "{" << std::endl;
  ss << "  successed : " << successed_ << std::endl;
  ss << "  error_text : " << error_text_ << std::endl;
  ss << "  is_canceled : " << is_canceled_ << std::endl;
  ss << "  id : " << id_ << std::endl;
  if (method_desc_ != NULL) {
  ss << "  method_desc : {\n\t" << method_desc_->DebugString() << "\n}" << std::endl;
  } else {
    ss << "  method_desc : NULL" << std::endl;
  }
  if (request_ != NULL) {
    ss << "  request : {\n\t" << request_->DebugString() << "\n}" << std::endl;
  } else {
    ss << "  request : NULL" << std::endl;
  }
  if (response_ != NULL) {
    ss << "  response : {\n\t" << response_->DebugString() << "\n}" << std::endl;
  } else {
    ss << "  response : NULL" << std::endl;
  }
  ss << "}";
  return ss.str();
}

uint64 PocoRpcController::genernate_rpc_id() {
  Poco::ScopedLockWithUnlock<Poco::FastMutex> lock(PocoRpcController::mutex_rpc_id_);
  PocoRpcController::last_rpc_id_++;
  return PocoRpcController::last_rpc_id_;
}

} // namespace
