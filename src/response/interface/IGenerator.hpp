#ifndef _IGENERATOR_HPP_
#define _IGENERATOR_HPP_

#include <sys/types.h>
#include <string>
#include <sys/select.h>

class IGenerator {
 private:
  unsigned short forceStatus;
 public:
  IGenerator ();
  virtual ~IGenerator ()
  {}
  virtual ssize_t Read (char *buf, size_t s, fd_set const *selectedFDs) = 0;
  virtual ssize_t getSize (fd_set const *selectedFDs, int *toCloseFds) = 0;
  virtual bool fullyHandled () const = 0;
  virtual void destroy () = 0;
  virtual IGenerator *clone () const = 0;
  unsigned short updateStatusCode (unsigned short status) const;
  void assignStatusCode (unsigned short status);
  virtual std::string genGetMime () const = 0;
};

#endif //_IGENERATOR_HPP_
