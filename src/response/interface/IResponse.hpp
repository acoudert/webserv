#ifndef _IRESPONSE_HPP_
#define _IRESPONSE_HPP_

#include <string>
#include <sys/select.h>

class IResponse {
 public:
  virtual ~IResponse ()
  {}

  virtual void setUp (int *internalFds) = 0;
  virtual char *
  getHeader (size_t *size, fd_set *selectedFDs, int *toCloseFds) = 0;
  virtual void headerSet (const std::string &key, const std::string &value) = 0;
  virtual bool execute (fd_set *selectedFDs, int *toCloseFds) = 0;
  virtual bool hasInternalFd (int *internalFds) const = 0;

  virtual IResponse *clone () const = 0;
  virtual void destroy () = 0;
};

#endif // _IRESPONSE_HPP_
