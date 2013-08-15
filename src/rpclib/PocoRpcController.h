/* 
 * File:   PocoRpcController.h
 * Author: kk
 *
 * Created on 2013年8月11日, 下午10:21
 */

#ifndef POCORPCCONTROLLER_H
#define	POCORPCCONTROLLER_H

#include "base/base.h"
#include "rpclib/PocoRpcChannel.h"

#include <google/protobuf/service.h>
#include <Poco/Mutex.h>
#include <Poco/Condition.h>

using std::string;
using google::protobuf::Closure;
using google::protobuf::Message;
using google::protobuf::MethodDescriptor;

namespace PocoRpc {

class PocoRpcController : public google::protobuf::RpcController {
 public:
  friend class PocoRpcChannel;
  
  virtual ~PocoRpcController();
  
    // Resets the RpcController to its initial state so that it may be reused in
  // a new call.  Must not be called while an RPC is in progress.
  virtual void Reset();

  // After a call has finished, returns true if the call failed.  The possible
  // reasons for failure depend on the RPC implementation.  Failed() must not
  // be called before a call has finished.  If Failed() returns true, the
  // contents of the response message are undefined.
  virtual bool Failed() const;

  // If Failed() is true, returns a human-readable description of the error.
  virtual string ErrorText();

  // Advises the RPC system that the caller desires that the RPC call be
  // canceled.  The RPC system may cancel it immediately, may wait awhile and
  // then cancel it, or may not even cancel the call at all.  If the call is
  // canceled, the "done" callback will still be called and the RpcController
  // will indicate that the call failed at that time.
  virtual void StartCancel();

  // Server-side methods ---------------------------------------------
  // These calls may be made from the server side only.  Their results
  // are undefined on the client side (may crash).

  // Causes Failed() to return true on the client side.  "reason" will be
  // incorporated into the message returned by ErrorText().  If you find
  // you need to return machine-readable information about failures, you
  // should incorporate it into your response protocol buffer and should
  // NOT call SetFailed().
  virtual void SetFailed(const string& reason);

  // If true, indicates that the client canceled the RPC, so the server may
  // as well give up on replying to it.  The server should still call the
  // final "done" callback.
  virtual bool IsCanceled();

  // Asks that the given callback be called when the RPC is canceled.  The
  // callback will always be called exactly once.  If the RPC completes without
  // being canceled, the callback will be called after completion.  If the RPC
  // has already been canceled when NotifyOnCancel() is called, the callback
  // will be called immediately.
  //
  // NotifyOnCancel() must be called no more than once per request.
  virtual void NotifyOnCancel(Closure* callback);

  void set_method_desc(const MethodDescriptor* method_desc);
  void set_request(const Message* request);
  void set_response(Message* response);
  void set_on_done_callback(Closure* on_done_callback);
  
  void wait();
  // return false if timeout
  bool tryWait(long milliseconds);
  // signal for rpc finished or canceled
  void signal_rpc_over();
  
  string DebugString();
  
 private:
  PocoRpcChannel* poco_rpc_ch_;
  bool successed_;
  string error_text_;
  bool is_canceled_;
  Closure* on_cancel_callback_;
  uint64 id_;
  const MethodDescriptor* method_desc_;
  const Message* request_;
  Message* response_;
  Closure* on_done_callback_;
  scoped_ptr<Poco::FastMutex> rpc_condt_mutex_;
  scoped_ptr<Poco::Condition> rpc_condt_;
  
  PocoRpcController(PocoRpcChannel* rpc_ch);
  
  static Poco::FastMutex mutex_rpc_id_;
  static uint64 last_rpc_id_;
  static uint64 genernate_rpc_id();
  
  DISALLOW_COPY_AND_ASSIGN(PocoRpcController);
};

Poco::FastMutex PocoRpcController::mutex_rpc_id_;
uint64 PocoRpcController::last_rpc_id_(0);

} // namespace

#endif	/* POCORPCCONTROLLER_H */

