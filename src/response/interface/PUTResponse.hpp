#ifndef _PUTRESPONSE_HPP_
#define _PUTRESPONSE_HPP_

#include "IResponse.hpp"
#include <string>
#include <sys/select.h>
#include <iostream>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdexcept>
#include <Response.hpp>

class PUTResponse : public IResponse {
 private:
  Response &resp;
  int outFd;
  const Location *loc;
  bool executed;
  ResponseHeader headers;
  size_t copyOffset;
  IGenerator *gen;
 public:
  ~PUTResponse ();
  PUTResponse (Response &resp, const Location &loc);
  PUTResponse (const PUTResponse &x);
  PUTResponse &operator= (const PUTResponse &x);

  void setUp (int *internalFds);
  char *getHeader (size_t *size, fd_set *selectedFDs, int *toCloseFds);
  void headerSet (const std::string &key, const std::string &value);
  bool execute (fd_set *selectedFDs, int *toCloseFds);
  bool hasInternalFd (int *internalFds) const;

  PUTResponse *clone () const;
  void destroy ();
 private:
  PUTResponse ();
};

#endif //_PUTRESPONSE_HPP_
