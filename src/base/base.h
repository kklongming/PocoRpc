#ifndef BASE_H
#define	BASE_H

#include <vector>
#include <deque>
#include <list>
#include <set>
#include <map>
#include <algorithm>
#include <string>
#include <sstream>
#include <iostream>

#include <stdint.h>
#include <gflags/gflags.h>
#include <glog/logging.h>

#include "scoped_ptr.h"

typedef int8_t int8;
typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

std::string int2string(int32 v);
std::string int2string(int64 v);
int32 str2int32(const std::string& str);
int64 str2int64(const std::string& str);

std::string Sha1String(const std::string& input_str);
std::string Md51String(const std::string& input_str);

std::string EncryptString(const std::string& val);
std::string DecryptString(const std::string& val);
bool tryDecryptString(const std::string& val, std::string* plain_txt);

std::string ReadFromFile(const std::string& file_path, bool strip=true);
bool WriteToFile(const std::string& file_path, const std::string& value);
std::string CatFile(const std::string& file_path, bool strip=true);

// A macro to disallow the copy constructor and operator= functions
// This should be used in the private: declarations for a class
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&); \
  void operator=(const TypeName&)

template<typename T>
void STLClear(T* v) {
  for (typename T::iterator it = v->begin(); it != v->end(); ++it) {
    delete *it;
  }
  v->clear();
}

template<typename T>
void STLMapClear(T* v) {
  for (typename T::iterator it = v->begin(); it != v->end(); ++it) {
    delete it->second;
  }
  v->clear();
}

template<typename T>
T Min(T x, T y) {
  if (x < y) {
    return x;
  } else {
    return y;
  }
}

template<typename T>
T Max(T x, T y) {
  if (x < y) {
    return y;
  } else {
    return x;
  }
}

#define FAIL(ret)   (ret != 0)
#define SUCCESS(ret)  (ret == 0)

#define CHECK_ARGS(arg)                             \
do {                                                \
  if (arg.empty()) {                                \
    std::string arg_name(#arg);                     \
      com::Result::set_result(result.get(),         \
            E_INVALID, "Missing --" + arg_name);    \
    std::cout << result->DebugString();             \
    return E_INVALID;                               \
  }                                                 \
} while(0)                                          \

#endif	/* BASE_H */

