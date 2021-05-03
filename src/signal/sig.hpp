#ifndef _SIG_HPP_
#define _SIG_HPP_

#include <csignal>

extern int _sock_fd;

void registerSocket (int sock);
void sigIntHandler (int sig);

void sigPipeHandler (int sig);

#endif //_SIG_HPP_
