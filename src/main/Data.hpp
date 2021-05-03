#ifndef _DATA_HPP_
#define _DATA_HPP_

#include <netinet/in.h>
#include <sys/time.h>
#include <vector>
#include <list>
#include <stdlib.h>
#include "ServerGroup.hpp"
#include "Global.hpp"

typedef struct sockaddr SOCKADDR;
typedef struct sockaddr_in SOCKADDR_IN;

struct Data {
 public:
  explicit Data (char **args);
  ~Data ();

  ServerGroup servers;
  std::vector<int> socket_fds;
  std::vector<SOCKADDR_IN> serverAddrs;
  std::list<int> connection_fds;

 private:
  Data ();
  Data (Data const &x);
  Data &operator= (Data const &x);

  void displayHelp () const;
};

#endif // _DATA_HPP_
