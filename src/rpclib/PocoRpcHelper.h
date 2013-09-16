/* 
 * File:   PocoRpcHelper.h
 * Author: kk
 *
 * Created on 2013年8月31日, 下午6:59
 */

#ifndef POCORPCHELPER_H
#define	POCORPCHELPER_H

#include "base/base.h"
#include <google/protobuf/service.h>

class AutoRunner {
 public:

  AutoRunner(::google::protobuf::Closure* obj)
          : obj_(obj) {
  }

  ~AutoRunner() {
    if (obj_ != NULL) {
      obj_->Run();
    }
  }

  ::google::protobuf::Closure* Release() {
    ::google::protobuf::Closure* obj = obj_;
    obj_ = NULL;
    return obj;
  }

 private:
  ::google::protobuf::Closure* obj_;

  DISALLOW_COPY_AND_ASSIGN(AutoRunner);
};

#endif	/* POCORPCHELPER_H */

