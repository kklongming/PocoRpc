/* 
 * File:   RpcSession.h
 * Author: kk
 *
 * Created on 2013年9月2日, 下午2:20
 */

#ifndef RPCSESSION_H
#define	RPCSESSION_H

#include "base/base.h"
#include "base/fifo_queue.h"
#include "rpc_proto/poco_rpc.pb.h"
#include <Poco/Timestamp.h>
#include <tr1/memory>

namespace PocoRpc {

typedef std::tr1::shared_ptr<RpcMessage> RpcMessagePtr;

class RpcSession {
 public:
  friend class RpcServiceHandler;
  
  RpcSession(const std::string& uuid, int timeout_in_ms);
  virtual ~RpcSession();

  bool expired();
  const std::string& uuid();
  
  void push(RpcMessagePtr rpcmsg);
  bool tryPopup(RpcMessagePtr* p_rpcmsg, int timeout);
  void ReleaseSendingRpcmsg();
  
  void reg_on_pushed_cb(Poco::Runnable* cb);
  void clear_on_pushed_cb();
  void reg_on_popuped_cb(Poco::Runnable* cb);
  void clear_on_popuped_cb();
  
  typedef FifoQueue<RpcMessagePtr> RpcMsgQueue;
  
 private:
  
  std::string client_uuid_;
  
  // session timeout in microseconds 
  int64 timeout_;
  
  Poco::Timestamp last_access_time_;
  
  /// 等待发送的response message
  scoped_ptr<RpcMsgQueue> pending_response_;
  RpcMessagePtr sending_rpcmsg_;

  inline void update_last_atime();
  
  DISALLOW_COPY_AND_ASSIGN(RpcSession);
};

typedef std::tr1::shared_ptr<RpcSession> RpcSessionPtr;

} // namespace PocoRpc

#endif	/* RPCSESSION_H */

