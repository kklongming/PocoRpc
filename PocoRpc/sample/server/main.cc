#include <PocoRpc/base/base.h>
#include <PocoRpc/RpcServer.h>

#include "PocoRpc/sample/server/EchoService.h"

DECLARE_int32(rpc_worker_count_);

using namespace std;
using namespace PocoRpc;

void RunRpcServer() {
  scoped_ptr<PocoRpcServer> server(new PocoRpcServer(9999));
  Sample::EchoService* echo_svc = new Sample::EchoService_Imp();
  server->reg_service(echo_svc);
  server->run();
  server->waitForTerminationRequest();
  server->exit();
}

int main(int argc, char* argv[]) {
  // google::SetVersionString(UFS_VERSION);
  google::InitGoogleLogging(argv[0]);
  google::ParseCommandLineFlags(&argc, &argv, true);
  FLAGS_logtostderr = true;
  FLAGS_rpc_worker_count_ = 8;
  LOG(INFO) << "==> Start Rpc Server...";
  
  RunRpcServer();
  
  LOG(INFO) << "==> Exit Rpc Server...";

  return 0;
}