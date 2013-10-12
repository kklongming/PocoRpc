/* 
 * File:   daemon.cc
 * Author: kk
 * 
 * Created on 2013年8月31日, 下午1:14
 */

#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <signal.h>

#include <fstream>

#include <Poco/TemporaryFile.h>
#include <Poco/Process.h>
#include <Poco/Exception.h>

#include "base.h"
#include "daemon.h"

void runAsDaemon() {
  pid_t pid;
  if ((pid = fork()) < 0) {
    throw Poco::SystemException("cannot fork daemon process");
  } else if (pid != 0) {
    exit(0);
  }
  setsid();
  umask(0);

  // attach stdin, stdout, stderr to /dev/null
  // instead of just closing them. This avoids
  // issues with third party/legacy code writing
  // stuff to stdout/stderr.
  FILE* fin = freopen("/dev/null", "r+", stdin);
  if (!fin) throw Poco::OpenFileException("Cannot attach stdin to /dev/null");
  FILE* fout = freopen("/dev/null", "r+", stdout);
  if (!fout) throw Poco::OpenFileException("Cannot attach stdout to /dev/null");
  FILE* ferr = freopen("/dev/null", "r+", stderr);
  if (!ferr) throw Poco::OpenFileException("Cannot attach stderr to /dev/null");
}

void handlePidFile(const std::string& pid_file_path) {
  std::ofstream ostr(pid_file_path.c_str());
  if (ostr.good())
    ostr << Poco::Process::id() << std::endl;
  else
    throw Poco::CreateFileException("Cannot write PID to file", pid_file_path);
  Poco::TemporaryFile::registerForDeletion(pid_file_path);
}

void waitForTerminationRequest() {
  sigset_t sset;
  sigemptyset(&sset);

  sigaddset(&sset, SIGINT);
  sigaddset(&sset, SIGQUIT);
  sigaddset(&sset, SIGTERM);
  sigprocmask(SIG_BLOCK, &sset, NULL);
  int sig;
  sigwait(&sset, &sig);
}
