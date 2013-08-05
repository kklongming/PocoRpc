#include "base/base.h"

using namespace std;

int main(int argc, char* argv[]) {
  // google::SetVersionString(UFS_VERSION);
  google::InitGoogleLogging(argv[0]);
  google::ParseCommandLineFlags(&argc, &argv, true);

  cout << "this is rpc test server." << endl;

  return 0;
}