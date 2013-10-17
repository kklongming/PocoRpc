/* 
 * File:   MultiThreadTest.cc
 * Author: kk
 * 
 * Created on 2013年10月11日, 下午9:49
 */

#include "MultiThreadTest.h"

#include <Poco/Timestamp.h>
#include <Poco/Format.h>
#include <Poco/UUIDGenerator.h>
#include <Poco/ScopedLock.h>
#include "PocoRpc/sample/rpc_define/echo_service.pb.h"
#include "PocoRpc/rpclib/PocoRpcController.h"

DECLARE_string(rpc_server);
DECLARE_int32(rpc_port);

MultiThreadTest::MultiThreadTest(int thread_count) : thread_count_(thread_count),
    exit_(true), count_0_50_(0), count_50_100_(0), count_100_250_(0),
    count_250_500_(0), count_500_1000_(0), count_time_out_(0) {
  mutex_count_.reset(new Poco::FastMutex());
  thread_list_.reset(new ThreadPtrVec());
  ra_test_.reset(Poco::NewPermanentCallback(this, &MultiThreadTest::test));
  rpc_ch_.reset(new PocoRpc::PocoRpcChannel(FLAGS_rpc_server, FLAGS_rpc_port));
  CHECK(rpc_ch_->Init());
  echo_svc_stub_.reset(new Sample::EchoService_Stub(rpc_ch_.get()));
}

MultiThreadTest::~MultiThreadTest() {
  if (not exit_) {
    stop();
  }
}

void MultiThreadTest::start() {
  if (exit_) {
    exit_ = false;
    start_time_.reset(new Poco::Timestamp());
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
    stop_time_.reset(new Poco::Timestamp());
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
  if (start_time_.get() != NULL and stop_time_.get() != NULL) {
    Poco::Timestamp::TimeDiff test_time = *stop_time_ - *start_time_;
    ss << Poco::format("total time : %f s\n", test_time/1000000.0);
    ss << Poco::format("speed : %f rpc/s\n", total_count/(test_time/1000000.0));
  }

  return ss.str();
}

void MultiThreadTest::test() {
  LOG(INFO) << "TEST THREAD START...";
  while (not exit_) {
    Poco::Timestamp call_start;
    Sample::EchoReq request;
    Sample::EchoReply reply;

    Poco::UUID uuid = Poco::UUIDGenerator::defaultGenerator().create();
    request.set_msg(uuid.toString());

    PocoRpc::AutoPocoRpcControllerPtr rpc_ctr = rpc_ch_->NewRpcController();
    echo_svc_stub_->Echo(rpc_ctr.get(), &request, &reply, NULL);
    bool ret = rpc_ctr->tryWait(1000);
    Poco::Timestamp call_finished;
    int rpc_time = (call_finished - call_start)/1000;
    {
      Poco::ScopedLock<Poco::FastMutex> lock(*mutex_count_);
      if (ret) {
        if (rpc_time <= 50) {
          count_0_50_++;
        }
        if (rpc_time > 50 and rpc_time <=100) {
          count_50_100_++;
        }
        if (rpc_time > 100 and rpc_time <= 250) {
          count_100_250_++;
        }
        if (rpc_time > 250 and rpc_time <= 500) {
          count_250_500_++;
        }
        if (rpc_time > 500 and rpc_time <=1000) {
          count_500_1000_++;
        }
        CHECK(reply.msg() == uuid.toString());
      } else {
        count_time_out_++;
      }
    }
  }
  LOG(INFO) << "TEST THREAD EXIT...";
}

