#include "base/base.h"

#include "base/closure.h"
#include "base/runable.h"
#include "rpclib/PocoRpcController.h"
#include "rpclib/PocoRpcChannel.h"

#include "rpc_def/base_service.pb.h"

using namespace std;
using namespace PocoRpc;


void test_1() {
  scoped_ptr<PocoRpcChannel> ch(new PocoRpcChannel("localhost", 28000));
  CHECK(ch->Connect()) << "Faild to connect with rpc server.";
  PocoRpcController* rpcctr = ch->NewRpcController();
  
  scoped_ptr<BaseService_Stub> bservice(new BaseService_Stub(ch.get()));
  
  PingReq req;
  PingReply reply;
  
  bservice->Ping(rpcctr, &req, &reply, NULL);
  
  LOG(INFO) << ch->DebugString();
  
}

int main(int argc, char* argv[]) {
  // google::SetVersionString(PROG_VERSION);
  google::InitGoogleLogging(argv[0]);
  google::ParseCommandLineFlags(&argc, &argv, true);

  FLAGS_logtostderr = true;

  test_1();
  
  LOG(INFO) << "==> finished the test.";

  return 0;
}