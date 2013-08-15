/* 
 * File:   TaskQueue.cc
 * Author: kk
 * 
 * Created on 2013年8月15日, 下午3:07
 */

#include "TaskQueue.h"
#include <Poco/ScopedLock.h>

TaskQueue::TaskQueue() : can_add_task_(false), exit_(true) {
  pending_tasks_.reset(new SafeQueue<Poco::Runnable*>());
  mutex_.reset(new Poco::FastMutex());
  worker_.reset(new Poco::Thread());
  ra_.reset(Poco::NewPermanentCallback(this, &TaskQueue::RunTask));
}

TaskQueue::~TaskQueue() {
  if (!exit_) {
    StopAndWaitFinishAllTasks();
  }
  CHECK(exit_);
  pending_tasks_->lock();
  STLClear(pending_tasks_.get());
  pending_tasks_->clear();
  pending_tasks_->unlock();

}

bool TaskQueue::AddTask(Poco::Runnable* task) {
  Poco::ScopedLockWithUnlock<Poco::FastMutex> lock(*mutex_);
  if (exit_) {
    return false;
  }
  pending_tasks_->push(task);
  return true;
}

void TaskQueue::Start() {
  {
    Poco::ScopedLockWithUnlock<Poco::FastMutex> lock(*mutex_);
    can_add_task_ = true;
    exit_ = false;
  }
  worker_->start(*ra_);
}

// drop all unfinished tasks

void TaskQueue::StopImmediately() {
  {
    Poco::ScopedLockWithUnlock<Poco::FastMutex> lock(*mutex_);
    can_add_task_ = false;
    if (exit_) {
      return;
    }
    exit_ = true;
  }
  worker_->join();
}

void TaskQueue::StopAndWaitFinishAllTasks() {
  {
    Poco::ScopedLockWithUnlock<Poco::FastMutex> lock(*mutex_);
    can_add_task_ = false;
    if (exit_) {
      return;
    }
    while (true) {
      pending_tasks_->lock();
      bool no_task = pending_tasks_->empty();
      pending_tasks_->unlock();
      if (no_task) {
        break;
      } else {
        Poco::Thread::sleep(100);
      }
    }
    exit_ = true;
  }
  worker_->join();
}

void TaskQueue::RunTask() {
  while (!exit_) {
    Poco::Runnable* task;
    if (pending_tasks_->tryPopup(&task, 500)) {
      task->run();
      delete task;
    }
  }
}
