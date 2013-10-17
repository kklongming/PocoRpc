/* 
 * File:   RpcSession.cc
 * Author: kk
 * 
 * Created on 2013年9月2日, 下午2:20
 */

#include "PocoRpc/rpclib/RpcSession.h"
#include "PocoRpc/rpclib/RpcServiceHandler.h"
#include <Poco/ScopedLock.h>

namespace PocoRpc {

RpcSession::RpcSession(const std::string& uuid, int timeout_in_ms) :
client_uuid_(uuid), timeout_(timeout_in_ms), last_access_time_(),
service_handler_(NULL) {
  pending_response_.reset(new RpcMsgQueue());
  mutex_service_handler_.reset(new Poco::FastMutex());
}

RpcSession::~RpcSession() {
}

bool RpcSession::expired() {
  Poco::Timestamp now;
  Poco::Timestamp::TimeDiff diff = now - last_access_time_;
  return diff > timeout_;
}

const std::string& RpcSession::uuid() {
  return client_uuid_;
}

void RpcSession::push(RpcMessagePtr rpcmsg) {
  pending_response_->push(rpcmsg);
  update_last_atime();
}

bool RpcSession::tryPopup(RpcMessagePtr* p_rpcmsg, int timeout) {
  bool ret = pending_response_->tryPopup(p_rpcmsg, timeout);
  update_last_atime();
  if (ret) {
    sending_rpcmsg_ = *p_rpcmsg;
  }
  return ret;
}

void RpcSession::ReleaseSendingRpcmsg() {
  sending_rpcmsg_.reset();
}

void RpcSession::reg_on_pushed_cb(Poco::Runnable* cb) {
  pending_response_->reg_on_pushed_callback(cb);
}

void RpcSession::clear_on_pushed_cb() {
  pending_response_->clear_on_pushed_callback();
}

void RpcSession::reg_on_popuped_cb(Poco::Runnable* cb) {
  pending_response_->reg_on_popuped_callback(cb);
}

void RpcSession::clear_on_popuped_cb() {
  pending_response_->clear_on_popuped_callback();
}

void RpcSession::update_last_atime() {
  Poco::Timestamp now;
  last_access_time_ = now;
}

void RpcSession::reset_service_handler(RpcServiceHandler* handler) {
  Poco::ScopedLock<Poco::FastMutex> lock(*mutex_service_handler_);
  CHECK(service_handler_ != handler);
  if (service_handler_ != NULL) {
    delete service_handler_;
  }
  service_handler_ = handler;
}

void RpcSession::release_service_handler(RpcServiceHandler* handler) {
  Poco::ScopedLock<Poco::FastMutex> lock(*mutex_service_handler_);
  if (service_handler_ == handler) {
    service_handler_ = NULL;
  }
}

} // namespace PocoRpc {
