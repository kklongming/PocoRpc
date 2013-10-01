#include "base/base.h"

#include "base/closure.h"
#include "base/runable.h"
#include "rpclib/PocoRpcController.h"
#include "rpclib/PocoRpcChannel.h"
#include "rpc_def/base_service.pb.h"

#include <Poco/Thread.h>

using namespace std;
using namespace PocoRpc;

void DebugString(::google::protobuf::Message* msg) {
  LOG(INFO) << msg->GetDescriptor()->full_name() << " : {" << std::endl
          << msg->DebugString() << std::endl
          << "}";
}

void StartClient() {
  scoped_ptr<PocoRpcChannel> ch(new PocoRpcChannel("localhost", 9999));
  ch->init();
  CHECK(ch->Connect()) << "Faild to connect with rpc server.";

  scoped_ptr<BaseService_Stub> bservice(new BaseService_Stub(ch.get()));

  AutoPocoRpcControllerPtr ping_ctr = ch->NewRpcController();
  PingReq req;
  PingReply reply;

  bservice->Ping(ping_ctr.get(), &req, &reply, NULL);
  
  Poco::Thread::sleep(1000*15);

  AutoPocoRpcControllerPtr get_svc_list_ctr2 = ch->NewRpcController();

  GetServiceListReq get_service_rep;
  GetServiceListReply get_service_reply;

  bservice->GetServiceList(get_svc_list_ctr2.get(), &get_service_rep, &get_service_reply, NULL);

  

  get_svc_list_ctr2->wait();
  DebugString(&get_service_reply);
  
  ping_ctr->wait();
  DebugString(&reply);

}

int main(int argc, char* argv[]) {
  // google::SetVersionString(UFS_VERSION);
  google::InitGoogleLogging(argv[0]);
  google::ParseCommandLineFlags(&argc, &argv, true);
  FLAGS_logtostderr = true;

  LOG(INFO) << "==> Start Rpc Client...";
  StartClient();
  LOG(INFO) << "==> Exit Rpc Client...";

  return 0;
}