#include "sig.hpp"
#include <unistd.h>
#include <cstdlib>

int _sock_fd = -1;

void registerSocket (int sock)
{
  _sock_fd = sock;
}

void sigIntHandler (int sig)
{
  if (sig != SIGINT)
	return;
  if (_sock_fd != -1)
	close (_sock_fd);
  exit (-1);
}

void sigPipeHandler (int sig)
{
  (void) (sig);
}
