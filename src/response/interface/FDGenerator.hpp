#ifndef _FDGENERATOR_HPP_
#define _FDGENERATOR_HPP_

#include "IGenerator.hpp"
#include <sys/select.h>
#include <sys/types.h>
#include <iostream>
#include <cstdlib>

class FDGenerator : public IGenerator {
 private:
  std::string path;
  int fd;
  ssize_t bytesRead;

  FDGenerator ();
 public:
  FDGenerator (int const &_fd, std::string const &_path);
  ~FDGenerator ();
  explicit FDGenerator (const FDGenerator &x);
  FDGenerator &operator= (const FDGenerator &x);

  ssize_t Read (char *buf, size_t s, fd_set const *selectedFDs);
  ssize_t getSize (fd_set const *selectedFDs, int *toCloseFds);
  bool fullyHandled () const;
  void destroy ();
  IGenerator *clone () const;
  std::string genGetMime () const;
};

#endif //_FDGENERATOR_HPP_
