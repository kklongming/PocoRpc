/* 
 * File:   PocoRpcController.cc
 * Author: kk
 * 
 * Created on 2013年8月11日, 下午10:21
 */

#include "rpclib/PocoRpcController.h"
#include "rpclib/PocoRpcChannel.h"
#include "rpclib/BytesBuffer.h"
#include "rpc_proto/poco_rpc.pb.h"

#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>
#include <Poco/ScopedLock.h>

namespace PocoRpc {

Poco::FastMutex PocoRpcController::mutex_rpc_id_;
uint64 PocoRpcController::last_rpc_id_(0);

PocoRpcController::PocoRpcController(PocoRpcChannel* rpc_ch) :
poco_rpc_ch_(rpc_ch), successed_(false), error_text_("Not called"),
is_canceled_(false), is_normal_done_(false), on_cancel_callback_(NULL),
method_desc_(NULL), request_(NULL), response_(NULL), on_done_callback_(NULL) {
  id_ = genernate_rpc_id();
  rpc_condt_mutex_.reset(new Poco::FastMutex());
  rpc_condt_.reset(new Poco::Condition());
  status_mutex_.reset(new Poco::FastMutex());
}

PocoRpcController::~PocoRpcController() {
  LOG(INFO) << "Destory PocoRpcController. id=" << id_;
}

void PocoRpcController::Reset() {
  successed_ = false;
  error_text_ = "Not called";
  is_canceled_ = false;
  on_cancel_callback_ = NULL;
  method_desc_ = NULL;
  request_ = NULL;
  response_ = NULL;
  on_done_callback_ = NULL;
  id_ = genernate_rpc_id();
  rpc_condt_mutex_.reset(new Poco::FastMutex());
  rpc_condt_.reset(new Poco::Condition());
}

bool PocoRpcController::Failed() const {
  return !successed_;
}

string PocoRpcController::ErrorText() const {
  return error_text_;
}

void PocoRpcController::StartCancel() {
  Poco::ScopedLockWithUnlock<Poco::FastMutex> lock(*status_mutex_);
  if (is_rpc_finished()) return;

  is_canceled_ = true;
  if (on_cancel_callback_ != NULL) {
    on_cancel_callback_->Run();
  }
  // TODO 在PocoRpcChannel Cancel掉对应的RpcController, 并做destory的相应操作
  poco_rpc_ch_->CancelRpc(id_);
  rpc_condt_->broadcast();
}

void PocoRpcController::SetFailed(const string& reason) {
  error_text_ = reason;
}

bool PocoRpcController::IsCanceled() const {
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

uint64 PocoRpcController::id() {
  return id_;
}

BytesBuffer* PocoRpcController::NewBytesBuffer() {
  scoped_ptr<RpcMessage> rpc_msg(new RpcMessage());
  rpc_msg->set_id(id_);
  rpc_msg->set_method_full_name(method_desc_->full_name());
  rpc_msg->set_message_body(request_->SerializeAsString());

  BytesBuffer* rpc_buf = new BytesBuffer(rpc_msg->ByteSize());
  rpc_msg->SerializeToArray(reinterpret_cast<void*> (rpc_buf->pbody()),
          rpc_msg->ByteSize());
  return rpc_buf;
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
  Poco::ScopedLockWithUnlock<Poco::FastMutex> lock(*status_mutex_);
  if (is_rpc_finished()) return;
  is_normal_done_ = true;
  rpc_condt_->broadcast();
}

bool PocoRpcController::is_rpc_finished() {
  if (is_canceled_) return true;
  // not canceled
  return is_normal_done_;
}

string PocoRpcController::DebugString() {
  std::stringstream ss;
  ss << "{" << std::endl;
  ss << "  successed : " << (successed_ ? "yes" : "no") << std::endl;
  ss << "  error_text : " << error_text_ << std::endl;
  ss << "  is_canceled : " << (is_canceled_ ? "yes" : "no") << std::endl;
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
