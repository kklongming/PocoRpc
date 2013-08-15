/* 
 * File:   SafeQueue.h
 * Author: kk
 *
 * Created on 2013年8月14日, 下午1:49
 */

#ifndef SAFEQUEUE_H
#define	SAFEQUEUE_H

#include "base/base.h"
#include <Poco/Mutex.h>
#include <Poco/Condition.h>
#include <Poco/ScopedLock.h>


template<typename T>
class FifoQueue {
 public:
  typedef typename std::deque<T>::iterator iterator;
  
  FifoQueue();
  virtual ~FifoQueue();
  
  void push(const T& value);
  T popup();
  bool tryPopup(T* pv, long milliseconds);
  void sign_for_ready();
  
  void lock();
  void unlock();
  
  iterator begin();
  iterator end();
  void erase(typename std::deque<T>::iterator pos);
  void clear() {
    queue_->clear();
  }
  
  bool empty() {
    return queue_->empty();
  }
  
  std::string DebugString();
  std::string DebugStringWithoutLock();
  
 private:
  scoped_ptr<Poco::FastMutex> mutex_;
  scoped_ptr<Poco::Condition> condt_for_ready_;
  
  scoped_ptr<typename std::deque<T> > queue_;

  DISALLOW_COPY_AND_ASSIGN(FifoQueue);
};

template<typename T>
FifoQueue<T>::FifoQueue() {
  mutex_.reset(new Poco::FastMutex());
  condt_for_ready_.reset(new Poco::Condition());
  queue_.reset(new std::deque<T>());
}
template<typename T>
FifoQueue<T>::~FifoQueue() {
  queue_->clear();
}

template<typename T>
void FifoQueue<T>::push(const T& value) {
  Poco::ScopedLockWithUnlock<Poco::FastMutex> lock(*mutex_);
  bool need_sign_for_ready = queue_->empty();
  queue_->push_front(value);
  if (need_sign_for_ready) {
    sign_for_ready();
  }
}

template<typename T>
T FifoQueue<T>::popup() {
  Poco::ScopedLockWithUnlock<Poco::FastMutex> lock(*mutex_);
  bool need_wait_for_ready = queue_->empty();
  if (need_wait_for_ready) {
    condt_for_ready_->wait(*mutex_);
  }
  T value = queue_->back();
  queue_->pop_back();
  return value;
}

template<typename T>
bool FifoQueue<T>::tryPopup(T* pv, long milliseconds) {
  Poco::ScopedLockWithUnlock<Poco::FastMutex> lock(*mutex_);
  bool need_wait_for_ready = queue_->empty();
  if (need_wait_for_ready) {
    bool in_time = condt_for_ready_->tryWait(*mutex_, milliseconds);
    if (!in_time) { 
      // time out
      return false;
    }
  }
  *pv = queue_->back();
  queue_->pop_back();
  return true;
}

template<typename T>
void FifoQueue<T>::sign_for_ready() {
  condt_for_ready_->signal();
}

template<typename T>
void FifoQueue<T>::lock() {
  mutex_->lock();
}

template<typename T>
void FifoQueue<T>::unlock() {
  mutex_->unlock();
}

template<typename T>
typename FifoQueue<T>::iterator FifoQueue<T>::begin() {
  return queue_->begin();
}

template<typename T>
typename FifoQueue<T>::iterator FifoQueue<T>::end() {
  return queue_->end();
}

template<typename T>
void FifoQueue<T>::erase(typename FifoQueue<T>::iterator pos) {
  queue_->erase(pos);
}

template<typename T>
std::string FifoQueue<T>::DebugString() {
  Poco::ScopedLockWithUnlock<Poco::FastMutex> lock(*mutex_);
  return DebugStringWithoutLock();
}

template<typename T>
std::string FifoQueue<T>::DebugStringWithoutLock() {
  typename FifoQueue<T>::iterator it = begin();
  std::stringstream ss;
  ss << "[";
  for (; it != end(); ++it) {
    ss << *it << ", ";
  }
  if (!queue_->empty()) {
    ss << "\b\b";
  }
  ss << "]";
  return ss.str();
}

#endif	/* SAFEQUEUE_H */

