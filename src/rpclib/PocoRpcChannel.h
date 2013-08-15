/* 
 * File:   PocoRpcChannel.h
 * Author: kk
 *
 * Created on 2013年8月12日, 下午7:33
 */

#ifndef POCORPCCHANNEL_H
#define	POCORPCCHANNEL_H

#include <google/protobuf/service.h>
#include "base/base.h"

namespace PocoRpc {

class PocoRpcChannel : public google::protobuf::RpcChannel {
 public:
  PocoRpcChannel();
  virtual ~PocoRpcChannel();
  
 private:

  DISALLOW_COPY_AND_ASSIGN(PocoRpcChannel);
};

} // namespace

#endif	/* POCORPCCHANNEL_H */

