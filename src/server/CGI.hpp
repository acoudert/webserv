#ifndef _CGI_HPP_
#define _CGI_HPP_

#include <string>
#include <iostream>
#include "utils.hpp"

class CGI {
 private:
  std::string wildcard;
  std::string pathExecutable;
  bool error;
 public:
  CGI ();
  CGI (std::ifstream &file, std::string &line);
  CGI (const CGI &x);
  ~CGI ();

  CGI &operator= (const CGI &x);
  bool matchPath (const std::string &path);
  bool matchFileName (const std::string &fileName);
  bool match (const std::string &path);

  void displayCGI () const;
  bool errorConf () const;

  std::string const &getWildcard () const;
  std::string const &getPathExecutable () const;

  bool extractCGI (std::ifstream &file, std::string &line);
};

#endif //_CGI_HPP_
