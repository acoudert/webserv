#ifndef _AUTOINDEXGENERATOR_HPP_
#define _AUTOINDEXGENERATOR_HPP_

#include "IGenerator.hpp"
#include "Buffer.hpp"
#include <dirent.h>
#include <sys/select.h>

#define AUTOINDEX_BUF 8192

class AutoIndexGenerator : public IGenerator {
  std::string path;
  DIR *dirFd;
  size_t size;
  Buffer buf;

  AutoIndexGenerator ();
 public:
  AutoIndexGenerator (std::string const &_path, std::string const &reqUrl);
  AutoIndexGenerator (const AutoIndexGenerator &x);
  AutoIndexGenerator &operator= (const AutoIndexGenerator &x);

  ssize_t Read (char *buf, size_t s, fd_set const *selectedFDs);
  ssize_t getSize (fd_set const *selectedFDs, int *toCloseFds);
  bool fullyHandled () const;
  IGenerator *clone () const;
  void destroy ();
  std::string genGetMime () const;
};

#endif //_AUTOINDEXGENERATOR_HPP_
