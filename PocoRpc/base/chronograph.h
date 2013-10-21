/* 
 * File:   chronograph.h
 * Author: kk
 *
 * Created on 2013年10月17日, 下午10:17
 */

#ifndef CHRONOGRAPH_H
#define	CHRONOGRAPH_H

#include "base.h"
#include <Poco/Timestamp.h>

class chronograph {
 public:
  chronograph();
  chronograph(const std::string& msg);
  virtual ~chronograph();

 private:
  std::string msg_;
  scoped_ptr<Poco::Timestamp> enter_in_;
  scoped_ptr<Poco::Timestamp> exit_out_;

  DISALLOW_COPY_AND_ASSIGN(chronograph);
};

#endif	/* CHRONOGRAPH_H */

