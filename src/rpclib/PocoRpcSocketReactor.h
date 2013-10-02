/* 
 * File:   PocoRpcSocketReactor.h
 * Author: kk
 *
 * Created on 2013年10月2日, 下午4:25
 */

#ifndef POCORPCSOCKETREACTOR_H
#define	POCORPCSOCKETREACTOR_H

#include "base/base.h"
#include <Poco/Net/SocketReactor.h>

namespace PocoRpc {

class PocoRpcSocketReactor : public Poco::Net::SocketReactor {
 public:
  PocoRpcSocketReactor();
  virtual ~PocoRpcSocketReactor();
  
  virtual void onIdle();
  
 private:
  DISALLOW_COPY_AND_ASSIGN(PocoRpcSocketReactor);
};

} // namespace PocoRpc {

#endif	/* POCORPCSOCKETREACTOR_H */

