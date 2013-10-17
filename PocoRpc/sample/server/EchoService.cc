/* 
 * File:   EchoService.cc
 * Author: kk
 * 
 * Created on 2013年10月17日, 下午3:33
 */

#include "EchoService.h"

namespace Sample {

EchoService_Imp::EchoService_Imp() {

}

EchoService_Imp::~EchoService_Imp() {

}

void EchoService_Imp::Echo(::google::protobuf::RpcController* controller,
                           const ::Sample::EchoReq* request,
                           ::Sample::EchoReply* response,
                           ::google::protobuf::Closure* done) {
  response->set_msg(request->msg());
  if (done != NULL) {
    done->Run();
  }
}

} // namespace Sample
