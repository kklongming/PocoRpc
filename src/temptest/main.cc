#include "base/base.h"

#include "base/closure.h"
#include "base/runable.h"
#include "base/task_queue.h"

using namespace std;

class Something {
 public:

  Something() : id_(0) {

  }

  virtual ~Something() {
  }

  void do_something(const string& msg) {
    LOG(INFO) << "do something " << id_++ << " times. msg: " << msg;
    Poco::Thread::sleep(500);
  }

 private:
  int id_;

  DISALLOW_COPY_AND_ASSIGN(Something);
};

void test_StoppableThread() {
  string msg = "C++ string";
  scoped_ptr<Something> smt(new Something());
  scoped_ptr<Poco::StoppableThread> stopable_th(new Poco::StoppableThread(
          Poco::NewPermanentCallback<Something, const string&>(smt.get(), &Something::do_something, msg)));
  stopable_th->start();
  Poco::Thread::sleep(5000);
  stopable_th->join();
}

class Task : public Poco::Runnable {
 public:

  Task(int id) : id_(id) {
  };

  virtual ~Task() {
//    LOG(INFO) << "$$ Destory task: " << id_;
  }

  virtual void run() {
    LOG(INFO) << Poco::Thread::current()->name() << " do task: " << id_;
    Poco::Thread::sleep(500);
  }
 private:
  int id_;

  DISALLOW_COPY_AND_ASSIGN(Task);
};

void test_task_queue() {
  scoped_ptr<TaskQueue> task_queue(new TaskQueue(8));
  task_queue->Start();
  for (int i = 0; i < 30; ++i) {
    Task* task = new Task(i);
    task_queue->AddTask(task);
  }
  LOG(INFO) << "**> Finish add 20 tasks.";
  Poco::Thread::sleep(10000);
  LOG(INFO) << "**> Stop TaskQueue";
  task_queue->StopImmediately();
//  task_queue->StopAndWaitFinishAllTasks();

}

int main(int argc, char* argv[]) {
  // google::SetVersionString(PROG_VERSION);
  google::InitGoogleLogging(argv[0]);
  google::ParseCommandLineFlags(&argc, &argv, true);

  FLAGS_logtostderr = true;

  test_task_queue();

  LOG(INFO) << "==> finished the test.";

  return 0;
}