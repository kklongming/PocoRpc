/* 
 * File:   TaskQueue.h
 * Author: kk
 *
 * Created on 2013年8月15日, 下午3:07
 */

#ifndef TASKQUEUE_H
#define	TASKQUEUE_H

#include "base/base.h"
#include "base/fifo_queue.h"
#include "base/runable.h"

// single thread task queue
class TaskQueue {
 public:
  TaskQueue(int worker_thread_count = 1);
  virtual ~TaskQueue();
  
  // take over Poco::Runnable object
  bool AddTask(Poco::Runnable* task);
  void Start();
  void StopImmediately(); // drop all unfinished tasks
  void StopAndWaitFinishAllTasks();
  
 private:
  bool can_add_task_;
  bool exit_;
  int worker_count_;
  
  typedef std::vector<Poco::Thread*> ThreadVector;
  
  scoped_ptr<FifoQueue<Poco::Runnable*> > pending_tasks_;
  scoped_ptr<Poco::FastMutex> mutex_;
  scoped_ptr<ThreadVector> workers_;
  scoped_ptr<Poco::Runnable> ra_;
  
  void RunTask();

  DISALLOW_COPY_AND_ASSIGN(TaskQueue);
};

#endif	/* TASKQUEUE_H */

