/* 
 * File:   RpcSession.h
 * Author: kk
 *
 * Created on 2013年9月2日, 下午2:20
 */

#ifndef RPCSESSION_H
#define	RPCSESSION_H

#include "PocoRpc/base/base.h"
#include "PocoRpc/base/fifo_queue.h"
#include "PocoRpc/rpc_proto/poco_rpc.pb.h"
#include <Poco/Timestamp.h>
#include <Poco/Mutex.h>

namespace PocoRpc {

class RpcServiceHandler;

typedef shared_ptr<RpcMessage> RpcMessagePtr;

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

  typedef FifoQueue<RpcMessagePtr> RpcMsgQueue;

 private:

  std::string client_uuid_;

  // session timeout in microseconds 
  int64 timeout_;

  Poco::Timestamp last_access_time_;

  /// 等待发送的response message
  scoped_ptr<RpcMsgQueue> pending_response_;
  RpcMessagePtr sending_rpcmsg_;

  RpcServiceHandler* service_handler_;
  scoped_ptr<Poco::FastMutex> mutex_service_handler_;


  inline void update_last_atime();

  void reset_service_handler(RpcServiceHandler* handler);
  void release_service_handler(RpcServiceHandler* handler);

  DISALLOW_COPY_AND_ASSIGN(RpcSession);
};

typedef shared_ptr<RpcSession> RpcSessionPtr;

} // namespace PocoRpc

#endif	/* RPCSESSION_H */

