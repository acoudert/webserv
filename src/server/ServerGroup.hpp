#ifndef _SERVERGROUP_HPP_
#define _SERVERGROUP_HPP_

#include <vector>
#include <cassert>
#include <fstream>
#include <string>
#include <iostream>
#include <cstdlib>
#include "Server.hpp"

#define DEFAULT_CONF    "./conf/default"

class ServerGroup {
 public:
  explicit ServerGroup (char const *file_name);
  ServerGroup (ServerGroup const &x);
  ~ServerGroup ();
  ServerGroup &operator= (ServerGroup const &x);

  const Server *getServer (const Request &r) const;
  Server &operator[] (size_t const &i);
  size_t getSize () const; // return servers size
  size_t getThrNb () const;

  bool errorConf () const;
  void displayServerGroup () const;

 private:
  std::vector<Server> servers;
  size_t thr_nb; // Number of thread from conf file
  bool thr_nb_defined;
  bool error; // True if bad conf file

  ServerGroup ();
  bool extractBlock (std::ifstream &file);
  bool extractThrNb (std::string &line);
};

#endif //_SERVERGROUP_HPP_
