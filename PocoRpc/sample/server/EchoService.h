/* 
 * File:   EchoService.h
 * Author: kk
 *
 * Created on 2013年10月17日, 下午3:33
 */

#ifndef ECHOSERVICE_H
#define	ECHOSERVICE_H

#include "PocoRpc/base/base.h"
#include "PocoRpc/sample/rpc_define/echo_service.pb.h"

namespace Sample {

class EchoService_Imp : public EchoService {
 public:
  EchoService_Imp();
  virtual ~EchoService_Imp();
  
  virtual void Echo(::google::protobuf::RpcController* controller,
                       const ::Sample::EchoReq* request,
                       ::Sample::EchoReply* response,
                       ::google::protobuf::Closure* done);
  
 private:

  DISALLOW_COPY_AND_ASSIGN(EchoService_Imp);
};

} // namespace Sample

#endif	/* ECHOSERVICE_H */

