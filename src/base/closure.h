#ifndef CLOSURE_H_
#define CLOSURE_H_

#ifdef WITH_PROTOBUF

#include <google/protobuf/stubs/common.h>

typedef google::protobuf::Closure Closure;

#else

class Closure {
  public:
    Closure() {
    }
    virtual ~Closure() {
    }

    virtual void Run() = 0;

  private:
    DISALLOW_COPY_AND_ASSIGN (Closure);
};

#endif

class FunctionClosure0 : public Closure {
  public:
    typedef void (*FunctionType)();

    FunctionClosure0(FunctionType function, bool self_deleting)
        : function_(function), self_deleting_(self_deleting) {
    }
    virtual ~FunctionClosure0();

    virtual void Run() {
      function_();
      if (self_deleting_) delete this;
    }

  private:
    FunctionType function_;
    bool self_deleting_;
};

template<typename Class>
class MethodClosure0 : public Closure {
  public:
    typedef void (Class::*MethodType)();

    MethodClosure0(Class* object, MethodType method, bool self_deleting)
        : object_(object), method_(method), self_deleting_(self_deleting) {
    }
    virtual ~MethodClosure0() {
    }

    virtual void Run() {
      (object_->*method_)();
      if (self_deleting_) delete this;
    }

  private:
    Class* object_;
    MethodType method_;
    bool self_deleting_;
};

template<typename Arg1>
class FunctionClosure1 : public Closure {
  public:
    typedef void (*FunctionType)(Arg1 arg1);

    FunctionClosure1(FunctionType function, bool self_deleting, Arg1 arg1)
        : function_(function), self_deleting_(self_deleting), arg1_(arg1) {
    }
    virtual ~FunctionClosure1() {
    }

    virtual void Run() {
      function_(arg1_);
      if (self_deleting_) delete this;
    }

  private:
    FunctionType function_;
    bool self_deleting_;
    Arg1 arg1_;
};

template<typename Class, typename Arg1>
class MethodClosure1 : public Closure {
  public:
    typedef void (Class::*MethodType)(Arg1 arg1);

    MethodClosure1(Class* object, MethodType method, bool self_deleting,
                   Arg1 arg1)
        : object_(object), method_(method), self_deleting_(self_deleting), arg1_(
            arg1) {
    }
    virtual ~MethodClosure1() {
    }

    virtual void Run() {
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
class FunctionClosure2 : public Closure {
  public:
    typedef void (*FunctionType)(Arg1 arg1, Arg2 arg2);

    FunctionClosure2(FunctionType function, bool self_deleting, Arg1 arg1,
                     Arg2 arg2)
        : function_(function), self_deleting_(self_deleting), arg1_(arg1), arg2_(
            arg2) {
    }
    virtual ~FunctionClosure2() {
    }

    virtual void Run() {
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
class MethodClosure2 : public Closure {
  public:
    typedef void (Class::*MethodType)(Arg1 arg1, Arg2 arg2);

    MethodClosure2(Class* object, MethodType method, bool self_deleting,
                   Arg1 arg1, Arg2 arg2)
        : object_(object), method_(method), self_deleting_(self_deleting), arg1_(
            arg1), arg2_(arg2) {
    }
    virtual ~MethodClosure2() {
    }

    virtual void Run() {
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
class MethodClosure3 : public Closure {
  public:
    typedef void (Class::*MethodType)(Arg1 arg1, Arg2 arg2, Arg3 arg3);

    MethodClosure3(Class* object, MethodType method, bool self_deleting,
                   Arg1 arg1, Arg2 arg2, Arg3 arg3)
        : object_(object), method_(method), self_deleting_(self_deleting), arg1_(
            arg1), arg2_(arg2), arg3_(arg3) {
    }
    virtual ~MethodClosure3() {
    }

    virtual void Run() {
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
class MethodClosure4 : public Closure {
  public:
    typedef void (Class::*MethodType)(Arg1 arg1, Arg2 arg2, Arg3 arg3,
                                      Arg4 arg4);

    MethodClosure4(Class* object, MethodType method, bool self_deleting,
                   Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4)
        : object_(object), method_(method), self_deleting_(self_deleting), arg1_(
            arg1), arg2_(arg2), arg3_(arg3), arg4_(arg4) {
    }
    virtual ~MethodClosure4() {
    }

    virtual void Run() {
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

// See Closure.
inline Closure* NewCallback(void (*function)()) {
  return new FunctionClosure0(function, true);
}

// See Closure.
inline Closure* NewPermanentCallback(void (*function)()) {
  return new FunctionClosure0(function, false);
}

// See Closure.
template<typename Class>
inline Closure* NewCallback(Class* object, void (Class::*method)()) {
  return new MethodClosure0<Class>(object, method, true);
}

// See Closure.
template<typename Class>
inline Closure* NewPermanentCallback(Class* object, void (Class::*method)()) {
  return new MethodClosure0<Class>(object, method, false);
}

// See Closure.
template<typename Arg1>
inline Closure* NewCallback(void (*function)(Arg1), Arg1 arg1) {
  return new FunctionClosure1<Arg1>(function, true, arg1);
}

// See Closure.
template<typename Arg1>
inline Closure* NewPermanentCallback(void (*function)(Arg1), Arg1 arg1) {
  return new FunctionClosure1<Arg1>(function, false, arg1);
}

// See Closure.
template<typename Class, typename Arg1>
inline Closure* NewCallback(Class* object, void (Class::*method)(Arg1),
                            Arg1 arg1) {
  return new MethodClosure1<Class, Arg1>(object, method, true, arg1);
}

// See Closure.
template<typename Class, typename Arg1>
inline Closure* NewPermanentCallback(Class* object, void (Class::*method)(Arg1),
                                     Arg1 arg1) {
  return new MethodClosure1<Class, Arg1>(object, method, false, arg1);
}

// See Closure.
template<typename Arg1, typename Arg2>
inline Closure* NewCallback(void (*function)(Arg1, Arg2), Arg1 arg1,
                            Arg2 arg2) {
  return new FunctionClosure2<Arg1, Arg2>(function, true, arg1, arg2);
}

// See Closure.
template<typename Arg1, typename Arg2>
inline Closure* NewPermanentCallback(void (*function)(Arg1, Arg2), Arg1 arg1,
                                     Arg2 arg2) {
  return new FunctionClosure2<Arg1, Arg2>(function, false, arg1, arg2);
}

// See Closure.
template<typename Class, typename Arg1, typename Arg2>
inline Closure* NewCallback(Class* object, void (Class::*method)(Arg1, Arg2),
                            Arg1 arg1, Arg2 arg2) {
  return new MethodClosure2<Class, Arg1, Arg2>(object, method, true, arg1, arg2);
}

// See Closure.
template<typename Class, typename Arg1, typename Arg2>
inline Closure* NewPermanentCallback(Class* object,
                                     void (Class::*method)(Arg1, Arg2),
                                     Arg1 arg1, Arg2 arg2) {
  return new MethodClosure2<Class, Arg1, Arg2>(object, method, false, arg1,
                                               arg2);
}

// See Closure.
template<typename Class, typename Arg1, typename Arg2, typename Arg3>
inline Closure* NewCallback(Class* object,
                            void (Class::*method)(Arg1, Arg2, Arg3), Arg1 arg1,
                            Arg2 arg2, Arg3 arg3) {
  return new MethodClosure3<Class, Arg1, Arg2, Arg3>(object, method, true, arg1,
                                                     arg2, arg3);
}

// See Closure.
template<typename Class, typename Arg1, typename Arg2, typename Arg3>
inline Closure* NewPermanentCallback(Class* object,
                                     void (Class::*method)(Arg1, Arg2, Arg3),
                                     Arg1 arg1, Arg2 arg2, Arg3 arg3) {
  return new MethodClosure3<Class, Arg1, Arg2, Arg3>(object, method, false,
                                                     arg1, arg2, arg3);
}

// See Closure.
template<typename Class, typename Arg1, typename Arg2, typename Arg3,
    typename Arg4>
inline Closure* NewCallback(Class* object,
                            void (Class::*method)(Arg1, Arg2, Arg3, Arg4),
                            Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4) {
  return new MethodClosure4<Class, Arg1, Arg2, Arg3, Arg4>(object, method, true,
                                                           arg1, arg2, arg3,
                                                           arg4);
}

// See Closure.
template<typename Class, typename Arg1, typename Arg2, typename Arg3,
    typename Arg4>
inline Closure* NewPermanentCallback(
    Class* object, void (Class::*method)(Arg1, Arg2, Arg3, Arg4), Arg1 arg1,
    Arg2 arg2, Arg3 arg3, Arg4 arg4) {
  return new MethodClosure4<Class, Arg1, Arg2, Arg3, Arg4>(object, method,
                                                           false, arg1, arg2,
                                                           arg3, arg4);
}

#endif /* CLOSURE_H_ */
