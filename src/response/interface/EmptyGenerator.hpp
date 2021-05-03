#ifndef _EMPTYGENERATOR_HPP_
#define _EMPTYGENERATOR_HPP_

#include "IGenerator.hpp"
#include <sys/select.h>

class EmptyGenerator : public IGenerator {
 public:
  EmptyGenerator ();

  ssize_t Read (char *buf, size_t s, fd_set const *selectedFDs);
  ssize_t getSize (fd_set const *selectedFDs, int *toCloseFds);
  bool fullyHandled () const;
  IGenerator *clone () const;
  void destroy ();
  std::string genGetMime () const;
};

#endif //_EMPTYGENERATOR_HPP_
