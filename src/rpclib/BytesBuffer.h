/* 
 * File:   BytesBuffer.h
 * Author: kk
 *
 * Created on 2013年8月17日, 下午4:48
 */

#ifndef BYTESBUFFER_H
#define	BYTESBUFFER_H

#include "base/base.h"

namespace PocoRpc {

class BytesBuffer {
 public:
  BytesBuffer(uint32 size);
  virtual ~BytesBuffer();
  
  char* phead();
  char* pbody();
  
  uint32 get_body_size();
  uint32 get_total_size();
  void set_done_size(uint32 size);
  uint32 get_done_size();
  
  std::string DebugString();
  
 private:
  bool finished_;
  uint32 body_size_;
  uint32 done_size_;
  scoped_array<char> buf_;

  DISALLOW_COPY_AND_ASSIGN(BytesBuffer);
};

} // namespace

#endif	/* BYTESBUFFER_H */

