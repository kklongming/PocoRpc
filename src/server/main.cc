#include "base/base.h"
#include "rpclib/PocoRpcServer.h"

DECLARE_int32(rpc_worker_count_);

using namespace std;
using namespace PocoRpc;

void RunRpcServer() {
  scoped_ptr<PocoRpcServer> server(new PocoRpcServer(9999));
  server->run();
  server->waitForTerminationRequest();
  server->exit();
}

int main(int argc, char* argv[]) {
  // google::SetVersionString(UFS_VERSION);
  google::InitGoogleLogging(argv[0]);
  google::ParseCommandLineFlags(&argc, &argv, true);
  FLAGS_logtostderr = true;
  FLAGS_rpc_worker_count_ = 4;
  LOG(INFO) << "==> Start Rpc Server...";
  
  RunRpcServer();
  
  LOG(INFO) << "==> Exit Rpc Server...";

  return 0;
}