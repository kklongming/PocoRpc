#include "PocoRpc/base/base.h"
#include "PocoRpc/base/closure.h"
#include "PocoRpc/base/runable.h"
#include "PocoRpc/RpcClient.h"
#include "PocoRpc/rpc_def/base_service.pb.h"

#include <Poco/Thread.h>

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


  bool ret = get_svc_list_ctr2->tryWait(1000*10);
  if (ret) {
  DebugString(&get_service_reply);
  } else {
    LOG(INFO) << "TIME OUT";
    LOG(INFO) << get_svc_list_ctr2->DebugString();
  }

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
  //  StartClient();
  LOG(INFO) << "==> Exit Rpc Client...";

  return 0;
}