#ifndef _RESPONSE_HPP_
#define _RESPONSE_HPP_

#include <sys/socket.h>
#include <sys/time.h>
#include <cstring>
#include <cassert>
#include <map>
#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <cstdlib>
#include "Buffer.hpp"
#include "RequestLine.hpp"
#include "Methods.hpp"
#include "utils.hpp"
#include "Request.hpp"
class Response;
#include "IResponse.hpp"
#include "GETResponse.hpp"
#include "HEADResponse.hpp"
#include "PUTResponse.hpp"
#include "ErrorResponse.hpp"

class Response {
 public:
  Response (Request const &_req);
  Response (Response const &src);
  ~Response ();

  bool fullySent () const;

  bool
  handleResponse (int const &fd, fd_set *selectedFDs, int *toAddFds, int *toCloseFds);

  void onWritable (int const &fd, fd_set const *selectedFDs);
  bool intoWriteBuf (fd_set *selectedFDs, int *toCloseFds);
  size_t writeAvailable () const;
  size_t writeBody (char *buf, size_t bufSize);

  bool shouldWriteSelect () const;
  bool cleanInternalData ();

  void displayResponse () const;

  Request &getRequest ();

  IResponse *getConstructor ();
  IResponse *headConstructor ();
  IResponse *putConstructor ();
  IResponse *postConstructor ();
  IResponse *deleteConstructor ();

  Response &operator= (Response const &src);
  void destroy ();
 private:
  Response ();

  Request req;
  bool _IGenSet;
  bool _headerSent;
  bool _fullyWritten;
  bool _fullySent;

  Buffer writeBuf;
  IResponse *IGen;

  bool setIGen (int *toAddFds);
  void setStandardIGen (int *toAddFds);
};

typedef IResponse *(Response::*ftConstructor) ();

#endif // _REQUEST_HPP_
