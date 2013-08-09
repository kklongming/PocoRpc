/* 
 * File:   popen.cc
 * Author: kk
 * 
 * Created on 2013年4月1日, 上午12:14
 */

#include "popen.h"
#include "pystring.h"
#include "base.h"
#include <Poco/PipeStream.h>


namespace com {

Popen::Popen(const std::string& cmd, bool using_shell) : cmd_(cmd),
using_shell_(using_shell), shell_("/bin/bash") {
  output_lines_.reset(new StringList());
}

Popen::~Popen() {
}

bool Popen::run(Result* result) {
  if (using_shell_) {
    scoped_ptr< StringList > args(new StringList());
    args->push_back("-c");
    args->push_back(cmd_);
    return exec_cmd(shell_, args.get(), result);
  } else {
    scoped_ptr< StringList > args(new StringList());
    pystring::split(cmd_, *args);
    std::string cmd = args->front();
    args->erase(args->begin());

    return exec_cmd(cmd, args.get(), result);
  }

  return true;
}

StringList* Popen::output_lines() {
  return output_lines_.get();
}

bool Popen::using_bash() {
  return using_shell_;
}

void Popen::set_using_shell(bool using_shell) {
  using_shell_ = using_shell;
}

const std::string& Popen::shell() {
  return shell_;
}

void Popen::set_shell(const std::string& shell_path) {
  shell_ = shell_path;
}

bool Popen::exec_cmd(const std::string& cmd, const StringList* args, Result* result) {
  scoped_ptr< Poco::Pipe > out_pipe(new Poco::Pipe());
  Poco::ProcessHandle ph = Poco::Process::launch(cmd, *args, NULL, out_pipe.get(), out_pipe.get());
  
  scoped_ptr< Poco::PipeInputStream > pis(new Poco::PipeInputStream(*out_pipe));

  while (!pis->eof()) {
    std::string line_buf;
    std::getline(*pis, line_buf);
    output_lines_->push_back(line_buf);
  }
  if (output_lines_->size() > 0) {
    if (pystring::isempty(output_lines_->back())) {
      output_lines_->pop_back();
    }
  }
  int ret = ph.wait();
  if (ret != 0) {
    std::string err_msg = pystring::join("\n", *output_lines_);
    Result::set_result(result, ret, err_msg);
  } else {
    Result::set_result(result, 0, "OK");
  }
  return ret == 0;
}

} // namespace jstor

