#ifndef _GETRESPONSE_HPP_
#define _GETRESPONSE_HPP_

#include <string>
#include <sys/select.h>
#include <iostream>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdexcept>
#include <cerrno>
#include "IResponse.hpp"
#include "StatusCode.hpp"
#include "FDGenerator.hpp"
#include "CGIGenerator.hpp"
#include "Request.hpp"
#include "Server.hpp"
#include "Location.hpp"
#include "ResponseHeader.hpp"

class Response;

class GETResponse : public IResponse {
 public:
  GETResponse (Server const &s, Request &r, RequestLine const &rL, const Location &loc, Response &response);
  GETResponse (GETResponse const &x);
  ~GETResponse ();
  GETResponse &operator= (GETResponse const &x);

  void setUp (int *internalFds); // return *add for GET request
  char *getHeader (size_t *size, fd_set *selectedFDs, int *toCloseFds);
  void headerSet (const std::string &key, const std::string &value);
  bool execute (fd_set *selectedFDs, int *toCloseFds);
  bool hasInternalFd (int *internalFds) const;
  static unsigned short getLStatErrorCode ();
  void destroy ();

  IResponse *clone () const;

 protected:
  Server const &server;
  Request &request;
  Location const &location;
  RequestLine const &requestLine;
  IGenerator *generator;
  int fd;
  int inPipe[2]; // cgi STDIN
  int outPipe[2]; // cgi STDOUT
  unsigned short statusCode;
  ResponseHeader resp;
  Response &response;

  GETResponse ();
  void tryPath (const std::string &path, bool isCGI);
};

#include "Response.hpp"

#endif // _GETRESPONSE_HPP_
