/* 
 * File:   MultiThreadTest.cc
 * Author: kk
 * 
 * Created on 2013年10月11日, 下午9:49
 */

#include "MultiThreadTest.h"

#include <Poco/Timestamp.h>
#include <Poco/Format.h>

MultiThreadTest::MultiThreadTest(int thread_count) : thread_count_(thread_count),
    exit_(true), count_0_50_(0), count_50_100_(0), count_100_250_(0),
    count_250_500_(0), count_500_1000_(0), count_time_out_(0) {
  mutex_count_.reset(new Poco::FastMutex());
  thread_list_.reset(new ThreadPtrVec());
  ra_test_.reset(Poco::NewPermanentCallback(this, &MultiThreadTest::test));
}

MultiThreadTest::~MultiThreadTest() {
  if (not exit_) {
    stop();
  }
}

void MultiThreadTest::start() {
  if (exit_) {
    exit_ = false;
    for (int i = 0; i < thread_count_; ++i) {
      ThreadPtr thptr(new Poco::Thread());
      thread_list_->push_back(thptr);
      thptr->start(*ra_test_);
    }
  }
}

void MultiThreadTest::stop() {
  if (not exit_) {
    exit_ = true;
    ThreadPtrVec::iterator it = thread_list_->begin();
    for (; it != thread_list_->end(); ++it) {
      ThreadPtr thptr = *it;
      thptr->join();
    }
    thread_list_->clear();
  }
}

std::string MultiThreadTest::TestResult() {
  int total_count = count_0_50_ + count_50_100_ + count_100_250_ +
      count_250_500_ + count_500_1000_ + count_time_out_;
  std::stringstream ss;
  ss << Poco::format("response time in 0~50 ms      : %d    %2.2f%%\n", count_0_50_,
                     count_0_50_ * 100.0 / total_count);
  ss << Poco::format("response time in 50_100 ms    : %d    %2.2f%%\n", count_50_100_, 
                     count_50_100_ * 100.0 / total_count);
  ss << Poco::format("response time in 100_250 ms   : %d    %2.2f%%\n", count_100_250_, 
                     count_100_250_ * 100.0 / total_count);
  ss << Poco::format("response time in 250_500 ms   : %d    %2.2f%%\n", count_250_500_, 
                     count_250_500_ * 100.0 / total_count);
  ss << Poco::format("response time in 500_1000 ms  : %d    %2.2f%%\n", count_500_1000_, 
                     count_500_1000_ * 100.0 / total_count);
  ss << Poco::format("response time out             : %d    %2.2f%%\n", count_time_out_, 
                     count_time_out_ * 100.0 / total_count);
  ss << "================================\n";
  ss << Poco::format("total : %d\n", total_count);

  return ss.str();
}

void MultiThreadTest::test() {
  while (not exit_) {

  }
}

