/* 
 * File:   chronograph.cc
 * Author: kk
 * 
 * Created on 2013年10月17日, 下午10:17
 */

#include "chronograph.h"
#include <Poco/Format.h>

chronograph::chronograph() : msg_("") {
  enter_in_.reset(new Poco::Timestamp);
  LOG(INFO) << Poco::format("ENTER IN. %s", msg_);
}

chronograph::chronograph(const std::string& msg) : msg_(msg) {
  enter_in_.reset(new Poco::Timestamp);
  LOG(INFO) << Poco::format("enter in. %s", msg_);
}

chronograph::~chronograph() {
  exit_out_.reset(new Poco::Timestamp);
  int total_time = (*exit_out_ - *enter_in_)/1000;
  LOG(INFO) << Poco::format("EXIT OUT. total %d ms. %s", total_time, msg_);
}

