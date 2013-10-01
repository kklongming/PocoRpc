#include "base/base.h"
#include "rpclib/PocoRpcServer.h"

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
  
  LOG(INFO) << "==> Start Rpc Server...";
  
  RunRpcServer();
  
  LOG(INFO) << "==> Exit Rpc Server...";

  return 0;
}