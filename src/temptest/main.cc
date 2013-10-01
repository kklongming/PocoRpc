#include "base/base.h"

#include "base/closure.h"
#include "base/runable.h"
#include "rpclib/PocoRpcController.h"
#include "rpclib/PocoRpcChannel.h"
#include "rpclib/PocoRpcServer.h"

#include "rpc_def/base_service.pb.h"

#include "rpclib/BytesBuffer.h"

using namespace std;
using namespace PocoRpc;

void StartClient() {
  scoped_ptr<PocoRpcChannel> ch(new PocoRpcChannel("localhost", 28000));
  CHECK(ch->Connect()) << "Faild to connect with rpc server.";
  AutoPocoRpcControllerPtr rpcctr = ch->NewRpcController();

  scoped_ptr<BaseService_Stub> bservice(new BaseService_Stub(ch.get()));

  PingReq req;
  PingReply reply;

  bservice->Ping(rpcctr.get(), &req, &reply, NULL);

  LOG(INFO) << ch->DebugString();

}

void RunRpcServer() {
  scoped_ptr<PocoRpcServer> server(new PocoRpcServer(9999));
  server->run();
  server->waitForTerminationRequest();
  server->exit();
}

void test_buf_size() {
  scoped_ptr<BytesBuffer> buf(new BytesBuffer(68));
  LOG(INFO) << "buf size=" << buf->get_body_size();
  char tmp_buf[4];
  tmp_buf[0] = *buf->phead();
  tmp_buf[1] = *(buf->phead() + 1);
  tmp_buf[2] = *(buf->phead() + 2);
  tmp_buf[3] = *(buf->phead() + 3);

  uint32* puint32 = (uint32*)(tmp_buf);
  LOG(INFO) << "net_uint32=" << *puint32;
  LOG(INFO) << (uint32)tmp_buf[0];
  LOG(INFO) << (uint32)tmp_buf[1];
  LOG(INFO) << (uint32)tmp_buf[2];
  LOG(INFO) << (uint32)tmp_buf[3];
  LOG(INFO) << "SIZE=" << ntohl(*puint32);
  
}

int main(int argc, char* argv[]) {
  // google::SetVersionString(PROG_VERSION);
  google::InitGoogleLogging(argv[0]);
  google::ParseCommandLineFlags(&argc, &argv, true);

  FLAGS_logtostderr = true;

  test_buf_size();

  LOG(INFO) << "==> finished the test.";

  return 0;
}