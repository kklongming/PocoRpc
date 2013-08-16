/* 
 * File:   TaskQueue.cc
 * Author: kk
 * 
 * Created on 2013年8月15日, 下午3:07
 */

#include "task_queue.h"
#include <Poco/ScopedLock.h>
#include <Poco/Format.h>

TaskQueue::TaskQueue(int worker_thread_count) : can_add_task_(false), exit_(true) {
  pending_tasks_.reset(new FifoQueue<Poco::Runnable*>());
  mutex_.reset(new Poco::FastMutex());
  workers_.reset(new TaskQueue::ThreadVector());
  ra_.reset(Poco::NewPermanentCallback(this, &TaskQueue::RunTask));

  for (int i = 0; i < worker_thread_count; ++i) {
    Poco::Thread* th = new Poco::Thread();
    th->setName(Poco::format("TaskQueue-Worker-%d", i));
    workers_->push_back(th);
  }
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

  STLClear(workers_.get());
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
    CHECK(exit_ == true);
    can_add_task_ = true;
    exit_ = false;
  }
  LOG(INFO) << "workers count: " << workers_->size();
  TaskQueue::ThreadVector::iterator it_worker = workers_->begin();
  for (; it_worker != workers_->end(); ++it_worker) {
    Poco::Thread* th = *it_worker;
    LOG(INFO) << "Start TaskQueue worker: " << th->getName();
    th->start(*ra_);
  }
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
  TaskQueue::ThreadVector::iterator it_worker = workers_->begin();
  for (; it_worker != workers_->end(); ++it_worker) {
    Poco::Thread* th = *it_worker;
    LOG(INFO) << "Stop TaskQueue worker: " << th->getName();
    th->join();
  }
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
  TaskQueue::ThreadVector::iterator it_worker = workers_->begin();
  for (; it_worker != workers_->end(); ++it_worker) {
    Poco::Thread* th = *it_worker;
    LOG(INFO) << "Stop TaskQueue worker: " << th->getName();
    th->join();
  }
}

void TaskQueue::RunTask() {
  while (!exit_) {
    Poco::Runnable* task;
    if (pending_tasks_->tryPopup(&task, 500)) {
      CHECK(task != NULL);
      task->run();
      delete task;
    }
  }
}
