/* 
 * File:   popen.h
 * Author: kk
 *
 * Created on 2013年4月1日, 上午12:14
 */

#ifndef POPEN_H
#define	POPEN_H

#include <Poco/Process.h>

#include "base.h"
#include "pystring.h"
#include "result.h"

namespace com {

typedef std::vector< std::string > StringList;

class Popen {
public:
  Popen(const std::string& cmd, bool using_shell = true);
  virtual ~Popen();
  
  bool run(Result* result = NULL);
  StringList* output_lines();
  
  bool using_bash();
  void set_using_shell(bool using_shell);
  const std::string& shell();
  void set_shell(const std::string& shell_path);
  
private:
  bool using_shell_;
  std::string shell_;
  scoped_ptr<StringList> output_lines_;
  std::string cmd_;
  
  bool exec_cmd(const std::string& cmd, const StringList* args, Result* result);

  DISALLOW_COPY_AND_ASSIGN(Popen);
  
};

} // namespace jstor

#endif	/* POPEN_H */

