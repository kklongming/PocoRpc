/* 
 * File:   BytesBuffer.cc
 * Author: kk
 * 
 * Created on 2013年8月17日, 下午4:48
 */

#include "BytesBuffer.h"
#include <arpa/inet.h>

namespace PocoRpc {

BytesBuffer::BytesBuffer(uint32 size) : body_size_(size), done_size_(0) {
  buf_.reset(new char[size + 4]);
  uint32 size_n = htonl(size);
  uint32* p_size_head = reinterpret_cast<uint32*> (phead());
  *p_size_head = size_n;
}

BytesBuffer::~BytesBuffer() {
}

char* BytesBuffer::phead() {
  return buf_.get();
}

char* BytesBuffer::pbody() {
  return buf_.get() + 4;
}

uint32 BytesBuffer::get_size() {
  return body_size_;
}

void BytesBuffer::set_done_size(uint32 size) {
  done_size_ = size;
}

uint32 BytesBuffer::get_done_size() {
  return done_size_;
}

} // namespace

