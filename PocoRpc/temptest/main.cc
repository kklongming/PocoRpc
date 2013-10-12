#include <errno.h>
#include <string.h>

#include "PocoRpc/base/base.h"
#include <Poco/Thread.h>

using namespace std;

int main(int argc, char* argv[]) {
  // google::SetVersionString(PROG_VERSION);
  google::InitGoogleLogging(argv[0]);
  google::ParseCommandLineFlags(&argc, &argv, true);

  FLAGS_logtostderr = true;
  
  LOG(INFO) << "==> finished the test.";

  return 0;
}