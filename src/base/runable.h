/* 
 * File:   runable.h
 * Author: kk
 *
 * Created on 2013年8月15日, 下午1:39
 */

#ifndef RUNABLE_H
#define	RUNABLE_H

#include "base/base.h"
#include <Poco/Runnable.h>
#include <Poco/Thread.h>

namespace Poco {

//using Poco::Runnable;

class FunctionRunnable0 : public Runnable {
 public:
  typedef void (*FunctionType)();

  FunctionRunnable0(FunctionType function, bool self_deleting)
          : function_(function), self_deleting_(self_deleting) {
  }
  virtual ~FunctionRunnable0();

  virtual void run() {
    function_();
    if (self_deleting_) delete this;
  }

 private:
  FunctionType function_;
  bool self_deleting_;
};

template<typename Class>
class MethodRunnable0 : public Runnable {
 public:
  typedef void (Class::*MethodType)();

  MethodRunnable0(Class* object, MethodType method, bool self_deleting)
          : object_(object), method_(method), self_deleting_(self_deleting) {
  }

  virtual ~MethodRunnable0() {
  }

  virtual void run() {
    (object_->*method_)();
    if (self_deleting_) delete this;
  }

 private:
  Class* object_;
  MethodType method_;
  bool self_deleting_;
};

template<typename Arg1>
class FunctionRunnable1 : public Runnable {
 public:
  typedef void (*FunctionType)(Arg1 arg1);

  FunctionRunnable1(FunctionType function, bool self_deleting, Arg1 arg1)
          : function_(function), self_deleting_(self_deleting), arg1_(arg1) {
  }

  virtual ~FunctionRunnable1() {
  }

  virtual void run() {
    function_(arg1_);
    if (self_deleting_) delete this;
  }

 private:
  FunctionType function_;
  bool self_deleting_;
  Arg1 arg1_;
};

template<typename Class, typename Arg1>
class MethodRunnable1 : public Runnable {
 public:
  typedef void (Class::*MethodType)(Arg1 arg1);

  MethodRunnable1(Class* object, MethodType method, bool self_deleting,
          Arg1 arg1)
          : object_(object), method_(method), self_deleting_(self_deleting), arg1_(
          arg1) {
  }

  virtual ~MethodRunnable1() {
  }

  virtual void run() {
    (object_->*method_)(arg1_);
    if (self_deleting_) delete this;
  }

 private:
  Class* object_;
  MethodType method_;
  bool self_deleting_;
  Arg1 arg1_;
};

template<typename Arg1, typename Arg2>
class FunctionRunnable2 : public Runnable {
 public:
  typedef void (*FunctionType)(Arg1 arg1, Arg2 arg2);

  FunctionRunnable2(FunctionType function, bool self_deleting, Arg1 arg1,
          Arg2 arg2)
          : function_(function), self_deleting_(self_deleting), arg1_(arg1), arg2_(
          arg2) {
  }

  virtual ~FunctionRunnable2() {
  }

  virtual void run() {
    function_(arg1_, arg2_);
    if (self_deleting_) delete this;
  }

 private:
  FunctionType function_;
  bool self_deleting_;
  Arg1 arg1_;
  Arg2 arg2_;
};

template<typename Class, typename Arg1, typename Arg2>
class MethodRunnable2 : public Runnable {
 public:
  typedef void (Class::*MethodType)(Arg1 arg1, Arg2 arg2);

  MethodRunnable2(Class* object, MethodType method, bool self_deleting,
          Arg1 arg1, Arg2 arg2)
          : object_(object), method_(method), self_deleting_(self_deleting), arg1_(
          arg1), arg2_(arg2) {
  }

  virtual ~MethodRunnable2() {
  }

  virtual void run() {
    (object_->*method_)(arg1_, arg2_);
    if (self_deleting_) delete this;
  }

 private:
  Class* object_;
  MethodType method_;
  bool self_deleting_;
  Arg1 arg1_;
  Arg2 arg2_;
};

template<typename Class, typename Arg1, typename Arg2, typename Arg3>
class MethodRunnable3 : public Runnable {
 public:
  typedef void (Class::*MethodType)(Arg1 arg1, Arg2 arg2, Arg3 arg3);

  MethodRunnable3(Class* object, MethodType method, bool self_deleting,
          Arg1 arg1, Arg2 arg2, Arg3 arg3)
          : object_(object), method_(method), self_deleting_(self_deleting), arg1_(
          arg1), arg2_(arg2), arg3_(arg3) {
  }

  virtual ~MethodRunnable3() {
  }

  virtual void run() {
    (object_->*method_)(arg1_, arg2_, arg3_);
    if (self_deleting_) delete this;
  }

 private:
  Class* object_;
  MethodType method_;
  bool self_deleting_;
  Arg1 arg1_;
  Arg2 arg2_;
  Arg3 arg3_;
};

template<typename Class, typename Arg1, typename Arg2, typename Arg3,
typename Arg4>
class MethodRunnable4 : public Runnable {
 public:
  typedef void (Class::*MethodType)(Arg1 arg1, Arg2 arg2, Arg3 arg3,
          Arg4 arg4);

  MethodRunnable4(Class* object, MethodType method, bool self_deleting,
          Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4)
          : object_(object), method_(method), self_deleting_(self_deleting), arg1_(
          arg1), arg2_(arg2), arg3_(arg3), arg4_(arg4) {
  }

  virtual ~MethodRunnable4() {
  }

  virtual void run() {
    (object_->*method_)(arg1_, arg2_, arg3_, arg4_);
    if (self_deleting_) delete this;
  }

 private:
  Class* object_;
  MethodType method_;
  bool self_deleting_;
  Arg1 arg1_;
  Arg2 arg2_;
  Arg3 arg3_;
  Arg4 arg4_;
};

// See Runnable.

inline Runnable* NewCallback(void (*function)()) {
  return new FunctionRunnable0(function, true);
}

// See Runnable.

inline Runnable* NewPermanentCallback(void (*function)()) {
  return new FunctionRunnable0(function, false);
}

// See Runnable.

template<typename Class>
inline Runnable* NewCallback(Class* object, void (Class::*method)()) {
  return new MethodRunnable0<Class>(object, method, true);
}

// See Runnable.

template<typename Class>
inline Runnable* NewPermanentCallback(Class* object, void (Class::*method)()) {
  return new MethodRunnable0<Class>(object, method, false);
}

// See Runnable.

template<typename Arg1>
inline Runnable* NewCallback(void (*function)(Arg1), Arg1 arg1) {
  return new FunctionRunnable1<Arg1>(function, true, arg1);
}

// See Runnable.

template<typename Arg1>
inline Runnable* NewPermanentCallback(void (*function)(Arg1), Arg1 arg1) {
  return new FunctionRunnable1<Arg1>(function, false, arg1);
}

// See Runnable.

template<typename Class, typename Arg1>
inline Runnable* NewCallback(Class* object, void (Class::*method)(Arg1),
        Arg1 arg1) {
  return new MethodRunnable1<Class, Arg1>(object, method, true, arg1);
}

// See Runnable.

template<typename Class, typename Arg1>
inline Runnable* NewPermanentCallback(Class* object, void (Class::*method)(Arg1),
        Arg1 arg1) {
  return new MethodRunnable1<Class, Arg1>(object, method, false, arg1);
}

// See Runnable.

template<typename Arg1, typename Arg2>
inline Runnable* NewCallback(void (*function)(Arg1, Arg2), Arg1 arg1,
        Arg2 arg2) {
  return new FunctionRunnable2<Arg1, Arg2>(function, true, arg1, arg2);
}

// See Runnable.

template<typename Arg1, typename Arg2>
inline Runnable* NewPermanentCallback(void (*function)(Arg1, Arg2), Arg1 arg1,
        Arg2 arg2) {
  return new FunctionRunnable2<Arg1, Arg2>(function, false, arg1, arg2);
}

// See Runnable.

template<typename Class, typename Arg1, typename Arg2>
inline Runnable* NewCallback(Class* object, void (Class::*method)(Arg1, Arg2),
        Arg1 arg1, Arg2 arg2) {
  return new MethodRunnable2<Class, Arg1, Arg2>(object, method, true, arg1, arg2);
}

// See Runnable.

template<typename Class, typename Arg1, typename Arg2>
inline Runnable* NewPermanentCallback(Class* object,
        void (Class::*method)(Arg1, Arg2),
        Arg1 arg1, Arg2 arg2) {
  return new MethodRunnable2<Class, Arg1, Arg2>(object, method, false, arg1,
          arg2);
}

// See Runnable.

template<typename Class, typename Arg1, typename Arg2, typename Arg3>
inline Runnable* NewCallback(Class* object,
        void (Class::*method)(Arg1, Arg2, Arg3), Arg1 arg1,
        Arg2 arg2, Arg3 arg3) {
  return new MethodRunnable3<Class, Arg1, Arg2, Arg3>(object, method, true, arg1,
          arg2, arg3);
}

// See Runnable.

template<typename Class, typename Arg1, typename Arg2, typename Arg3>
inline Runnable* NewPermanentCallback(Class* object,
        void (Class::*method)(Arg1, Arg2, Arg3),
        Arg1 arg1, Arg2 arg2, Arg3 arg3) {
  return new MethodRunnable3<Class, Arg1, Arg2, Arg3>(object, method, false,
          arg1, arg2, arg3);
}

// See Runnable.

template<typename Class, typename Arg1, typename Arg2, typename Arg3,
typename Arg4>
inline Runnable* NewCallback(Class* object,
        void (Class::*method)(Arg1, Arg2, Arg3, Arg4),
        Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4) {
  return new MethodRunnable4<Class, Arg1, Arg2, Arg3, Arg4>(object, method, true,
          arg1, arg2, arg3,
          arg4);
}

// See Runnable.

template<typename Class, typename Arg1, typename Arg2, typename Arg3,
typename Arg4>
inline Runnable* NewPermanentCallback(
        Class* object, void (Class::*method)(Arg1, Arg2, Arg3, Arg4), Arg1 arg1,
        Arg2 arg2, Arg3 arg3, Arg4 arg4) {
  return new MethodRunnable4<Class, Arg1, Arg2, Arg3, Arg4>(object, method,
          false, arg1, arg2,
          arg3, arg4);
}

class StoppableRunnable : public Runnable {
  public: 
    // take over Runnable object c.
    explicit StoppableRunnable(Runnable* c)
        : c_(c), exit_(false) {
    }

    virtual ~StoppableRunnable() {
    }

    virtual void run() {
      while (!exit_) {
        c_->run();
      }
    }

    void stop() {
      if (exit_) return;

      exit_ = true;
    }

  private:
    scoped_ptr<Runnable> c_;
    bool exit_;

    DISALLOW_COPY_AND_ASSIGN(StoppableRunnable);
};

class StoppableThread{
  public:
    // take over Runnable object c.
    explicit StoppableThread(Runnable* c) {
      stoppable_Runnable_.reset(new StoppableRunnable(c));
      thread_.reset(new Thread());
    }

    virtual ~StoppableThread() {
    }
    
    void start() {
      thread_->start(*stoppable_Runnable_);
    }

    virtual void join() {
      stoppable_Runnable_->stop();
      thread_->join();
    }

  private:
   scoped_ptr<StoppableRunnable> stoppable_Runnable_;
   scoped_ptr<Thread> thread_;

    DISALLOW_COPY_AND_ASSIGN(StoppableThread);
};

} // namespace

#endif	/* RUNABLE_H */

