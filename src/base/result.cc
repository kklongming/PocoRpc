/* 
 * File:   result.cc
 * Author: kk
 * 
 * Created on 2013年3月31日, 下午7:24
 */

#include "result.h"

namespace jstor {

Result::Result() : error_code_(0), error_msg_("OK") {
}

Result::~Result() {
}

int32 Result::error_code() {
  return error_code_;
}

void Result::set_error_code(int32 err_code) {
  error_code_ = err_code;
}

const std::string& Result::error_msg() {
  return error_msg_;
}

void Result::set_error_msg(const std::string& err_msg) {
  error_msg_ = err_msg;
}

std::string Result::DebugString() {
  std::stringstream ss;
  ss << "{" << std::endl;
  ss << "  ret : " <<  error_code_ << std::endl;
  ss << "  errmsg : " << error_msg_ << std::endl;
  ss << "}" << std::endl;
  return ss.str();
}

void Result::set_result(Result* result, int32 err_code, const std::string& error_msg) {
  if (result == NULL) {
    return;
  }
  result->set_error_code(err_code);
  result->set_error_msg(error_msg);
}

void Result::set_result(Result* result, int32 err_code, const char* error_msg) {
  if (result == NULL) {
    return;
  }
  result->set_error_code(err_code);
  result->error_msg_ = error_msg;
}

} // namespace jstor

