/* 
 * File:   daemon.h
 * Author: kk
 *
 * Created on 2013年8月31日, 下午1:14
 */

#ifndef DAEMON_H
#define	DAEMON_H

#include "base/base.h"

void runAsDaemon();
void handlePidFile(const std::string& pid_file_path);
void waitForTerminationRequest();

#endif	/* DAEMON_H */

