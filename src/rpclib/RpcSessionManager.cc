/* 
 * File:   RpcSessionManager.cc
 * Author: kk
 * 
 * Created on 2013年9月2日, 下午4:39
 */

#include "rpclib/RpcSessionManager.h"
#include "base/runable.h"

namespace PocoRpc {

RpcSessionManager::RpcSessionManager(uint32 timeout, uint32 check_interval) :
exit_(false), session_timeout_(timeout), check_interval_(check_interval) {
  session_map_.reset(new RpcSessionMap());
  session_map_rwlock_.reset(new Poco::RWLock());
  timeout_cb_queue_.reset(new CallbackQueue());
  timeout_cb_queue_mutex_.reset(new Poco::FastMutex());
  timer_for_check_session_.reset(new Poco::Timer(check_interval_, check_interval_));
  timer_cb_.reset(new Poco::TimerCallback<RpcSessionManager>(*this, &RpcSessionManager::on_timer));

  timer_for_check_session_->start(*timer_cb_);
}

RpcSessionManager::~RpcSessionManager() {
  exit_ = true;
  timer_for_check_session_->stop();
}

RpcSessionPtr RpcSessionManager::FindOrCreate(const std::string& uuid) {
  {
    Poco::ScopedReadRWLock read_lock(*session_map_rwlock_);
    RpcSessionMap::iterator it = session_map_->find(uuid);
    if (it != session_map_->end()) {
      return it->second;
    }
  }
  /// not found, so get write lock, and try find again. if still can not find
  /// it. Create a new RpcSession and add it to session_map_
  RpcSessionPtr session;
  {
    Poco::ScopedWriteRWLock write_lock(*session_map_rwlock_);
    RpcSessionMap::iterator it = session_map_->find(uuid);
    if (it != session_map_->end()) {
      return it->second;
    } else {
      /// still not find it by uuid
      /// so create a new one
//      session.assign(new RpcSession(uuid, session_timeout_));
      session.reset(new RpcSession(uuid, session_timeout_));
      session_map_->insert(std::pair<std::string, RpcSessionPtr>(uuid, session));
    }
  }
  /// add session timeout check task to schedule
  schedule_check_task(session);
  return session;
}

void RpcSessionManager::remove_session(const std::string& uuid) {
  Poco::ScopedWriteRWLock write_lock(*session_map_rwlock_);
  RpcSessionMap::iterator it = session_map_->find(uuid);
  if (it != session_map_->end()) {
    session_map_->erase(it);
  }
}

void RpcSessionManager::schedule_check_task(RpcSessionPtr rpc_session) {
  Poco::ScopedLock<Poco::FastMutex> lock(*timeout_cb_queue_mutex_);
  Poco::Runnable *task = Poco::NewCallback<RpcSessionManager, RpcSessionPtr>(this,
          &RpcSessionManager::check_session, rpc_session);
  timeout_cb_queue_->push(task);
}

void RpcSessionManager::check_session(RpcSessionPtr rpc_session) {
  if (rpc_session->expired()) {
    remove_session(rpc_session->uuid());
  } else {
    schedule_check_task(rpc_session);
  }
}

void RpcSessionManager::on_timer(Poco::Timer& timer) {
  while (true) {
    if (exit_) {
      return;
    }
    Poco::Runnable *task = NULL;
    {
      Poco::ScopedLock<Poco::FastMutex> lock(*timeout_cb_queue_mutex_);
      if (timeout_cb_queue_->empty()) {
        return;
      }
      // timeout_cb_queue_ is not empty.
      task = timeout_cb_queue_->front();
      timeout_cb_queue_->pop();
    }
    task->run();
  }
}


} // namespace PocoRpc {

