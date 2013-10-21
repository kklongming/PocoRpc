/* 
 * File:   RpcSessionManager.h
 * Author: kk
 *
 * Created on 2013年9月2日, 下午4:39
 */

#ifndef RPCSESSIONMANAGER_H
#define	RPCSESSIONMANAGER_H

#include "PocoRpc/rpclib/RpcSession.h"
#include "PocoRpc/base/base.h"

#include <queue>
#include <Poco/Thread.h>
#include <Poco/Runnable.h>
#include <Poco/RWLock.h>
#include <Poco/Timer.h>


namespace PocoRpc {

class RpcSessionManager {
 public:
  RpcSessionManager(uint32 timeout, uint32 check_interval);
  virtual ~RpcSessionManager();
  
  
  RpcSessionPtr FindOrCreate(const std::string& uuid);
  
 private:
  typedef std::map<std::string, RpcSessionPtr> RpcSessionMap;
  typedef std::list<Poco::Runnable*> CallbackQueue;
  
  bool exit_;
  uint32 session_timeout_;   // in milliseconds
  uint32 check_interval_;    // in milliseconds
  
  scoped_ptr<RpcSessionMap> session_map_;
  scoped_ptr<Poco::RWLock>  session_map_rwlock_;
  
  scoped_ptr<CallbackQueue> timeout_cb_queue_;
  scoped_ptr<Poco::FastMutex> timeout_cb_queue_mutex_;
  
  scoped_ptr<Poco::Timer> timer_for_check_session_;
  scoped_ptr<Poco::TimerCallback<RpcSessionManager> > timer_cb_;
  

  void remove_session(const std::string& uuid);
  void schedule_check_task(RpcSessionPtr rpc_session);
  void check_session(RpcSessionPtr rpc_session);
  
  void on_timer(Poco::Timer& timer);
  
  
  DISALLOW_COPY_AND_ASSIGN(RpcSessionManager);
};

} // namespace PocoRpc {

#endif	/* RPCSESSIONMANAGER_H */

