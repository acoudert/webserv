#ifndef _ERRORRESPONSE_HPP_
#define _ERRORRESPONSE_HPP_

#include <sys/select.h>
#include "IResponse.hpp"
#include "IGenerator.hpp"
#include "ErrorGenerator.hpp"
#include "Request.hpp"
#include "Response.hpp"

class ErrorResponse : public IResponse {
 private:
  unsigned short code;
  Response &r;
  IGenerator *gen;
  ResponseHeader header;
  ErrorResponse ();
 public:
  virtual ~ErrorResponse ();
  explicit ErrorResponse (Response &r, unsigned short code);
  ErrorResponse (const ErrorResponse &x);
  ErrorResponse &operator= (const ErrorResponse &x);
  void setUp (int *internalFds);
  char *getHeader (size_t *size, fd_set *selectedFDs, int *toCloseFds);
  void headerSet (const std::string &key, const std::string &value);
  bool execute (fd_set *selectedFDs, int *toCloseFds);
  bool hasInternalFd (int *internalFds) const;
  IResponse *clone () const;
  void destroy ();
};

#endif //_ERRORRESPONSE_HPP_
