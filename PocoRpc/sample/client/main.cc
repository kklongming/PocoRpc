#include "PocoRpc/base/base.h"
#include "PocoRpc/base/closure.h"
#include "PocoRpc/base/runable.h"
#include "PocoRpc/base/daemon.h"
#include "PocoRpc/RpcClient.h"
#include "PocoRpc/rpc_def/base_service.pb.h"

#include "MultiThreadTest.h"

#include <Poco/Thread.h>

#include <Poco/Timestamp.h>
#include <Poco/Format.h>
#include <Poco/UUIDGenerator.h>
#include "PocoRpc/sample/rpc_define/echo_service.pb.h"
#include "PocoRpc/rpclib/PocoRpcController.h"
#include "PocoRpc/rpclib/PocoRpcError.h"

DEFINE_string(rpc_server, "localhost", "rpc server host name or ip address");
DEFINE_int32(rpc_port, 9999, "rpc server port");

using namespace std;
using namespace PocoRpc;

void DebugString(::google::protobuf::Message* msg) {
  LOG(INFO) << msg->GetDescriptor()->full_name() << " : {" << std::endl
      << msg->DebugString() << std::endl
      << "}";
}

void StartClient() {
  scoped_ptr<PocoRpcChannel> ch(new PocoRpcChannel(FLAGS_rpc_server, FLAGS_rpc_port));
  CHECK(ch->Init()) << "Faild to connect with rpc server." << ch->DebugString();

  scoped_ptr<BaseService_Stub> bservice(new BaseService_Stub(ch.get()));

  AutoPocoRpcControllerPtr ping_ctr = ch->NewRpcController();
  PingReq req;
  PingReply reply;

  LOG(INFO) << "================= sleep(1000 * 10) =====================";
  Poco::Thread::sleep(1000 * 10);

  LOG(INFO) << "================== bservice->Ping ====================";
  bservice->Ping(ping_ctr.get(), &req, &reply, NULL);

  AutoPocoRpcControllerPtr get_svc_list_ctr2 = ch->NewRpcController();

  GetServiceListReq get_service_rep;
  GetServiceListReply get_service_reply;

  bservice->GetServiceList(get_svc_list_ctr2.get(), &get_service_rep, &get_service_reply, NULL);


  bool ret = get_svc_list_ctr2->tryWait(1000 * 10);
  if (ret) {
    DebugString(&get_service_reply);
  } else {
    LOG(INFO) << "TIME OUT";
    LOG(INFO) << get_svc_list_ctr2->DebugString();
  }

  ping_ctr->wait();
  DebugString(&reply);
}

void test2() {
  scoped_ptr<MultiThreadTest> test1(new MultiThreadTest(8));
  //  scoped_ptr<MultiThreadTest> test2(new MultiThreadTest(256));
  //  scoped_ptr<MultiThreadTest> test3(new MultiThreadTest(256));
  //  scoped_ptr<MultiThreadTest> test4(new MultiThreadTest(256));

  test1->start();
  //  test2->start();
  //  test3->start();
  //  test4->start();

//  waitForTerminationRequest();
  Poco::Thread::sleep(30*1000);

  test1->stop();
  //  test2->stop();
  //  test3->stop();
  //  test4->stop();

  std::cout << test1->TestResult() << std::endl;
  //  std::cout << test2->TestResult() << std::endl;
  //  std::cout << test3->TestResult() << std::endl;
  //  std::cout << test4->TestResult() << std::endl;
}

void test3() {
  scoped_ptr<PocoRpcChannel> rpc_ch_(new PocoRpcChannel(FLAGS_rpc_server, FLAGS_rpc_port));
  CHECK(rpc_ch_->Init()) << "Faild to connect with rpc server.";

  scoped_ptr<Sample::EchoService_Stub> echo_svc_stub_(new Sample::EchoService_Stub(rpc_ch_.get()));

  Sample::EchoReq request;
  Sample::EchoReply reply;

  Poco::UUID uuid = Poco::UUIDGenerator::defaultGenerator().create();
  request.set_msg(uuid.toString());

  PocoRpc::AutoPocoRpcControllerPtr rpc_ctr = rpc_ch_->NewRpcController();
  Poco::Thread::sleep(2000);
  Poco::Timestamp call_start;
  echo_svc_stub_->Echo(rpc_ctr.get(), &request, &reply, NULL);
  bool ret = rpc_ctr->tryWait(1000);
  Poco::Timestamp call_finished;
  int rpc_time = (call_finished - call_start) / 1000;
  LOG(INFO) << "rpc call_finished time = " << rpc_time << " ms" << std::endl;
}

int main(int argc, char* argv[]) {
  // google::SetVersionString(UFS_VERSION);
  google::InitGoogleLogging(argv[0]);
  google::ParseCommandLineFlags(&argc, &argv, true);
  FLAGS_logtostderr = true;

  LOG(INFO) << "==> Start Rpc Client...";
  test2();
  LOG(INFO) << "==> Exit Rpc Client...";

  return 0;
}