/* 
 * File:   MultiThreadTest.cc
 * Author: kk
 * 
 * Created on 2013年10月11日, 下午9:49
 */

#include "MultiThreadTest.h"

MultiThreadTest::MultiThreadTest(int thread_count) : thread_count_(thread_count),
        exit_(false) {
  thread_list_.reset(new ThreadPtrVec());
  ra_test_.reset(Poco::NewPermanentCallback(this, &MultiThreadTest::test));
}

MultiThreadTest::~MultiThreadTest() {
}

void MultiThreadTest::start() {

}

void MultiThreadTest::stop() {

}

void MultiThreadTest::test() {
  while (not exit_) {

  }
}

