/* 
 * File:   result.h
 * Author: kk
 *
 * Created on 2013年3月31日, 下午7:24
 */

#ifndef RESULT_H
#define	RESULT_H

#include "base.h"

namespace jstor {

class Result {
public:
  Result();
  virtual ~Result();

  int32 error_code();
  void set_error_code(int32 err_code);

  const std::string& error_msg();
  void set_error_msg(const std::string& err_msg);
  
  std::string DebugString();

  static void set_result(Result* result, int32 err_code, const std::string& error_msg);
  static void set_result(Result* result, int32 err_code, const char* error_msg);
private:
  int32 error_code_;
  std::string error_msg_;

  DISALLOW_COPY_AND_ASSIGN(Result);

};

} // namespace jstor

#endif	/* RESULT_H */

