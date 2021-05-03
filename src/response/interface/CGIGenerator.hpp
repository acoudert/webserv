#ifndef _CGIGENERATOR_HPP_
#define _CGIGENERATOR_HPP_

#include <sys/select.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <cstring>
#include <cstdlib>
#include <stdexcept>
#include <map>
#include <algorithm>
#include <cctype>
#include "IGenerator.hpp"
#include "Location.hpp"
#include "Server.hpp"
#include "Request.hpp"
#include "RequestLine.hpp"
#include "Methods.hpp"
#include "ResponseHeader.hpp"
#include "Global.hpp"

#define FIELD_NB  18

// PUT ALL CALLS IN TRY/CATCH
class CGIGenerator : public IGenerator {
 private:
  Server const &server;
  Request &request;
  RequestLine const &requestLine;
  Location const &location;
  char **argv;
  char **envp;
  pid_t pid;
  int fd;
  int *inPipe;
  int *outPipe;
  std::string file;
  std::string filePath;
  bool forkLaunched;
  bool forkStdin;
  std::string cgiPath;
  ResponseHeader &resp;
  bool handled;
  std::string *envStr;

  CGIGenerator ();
  void setUpArgv ();
  void setUpEnvp ();
  void setUpEnvStr ();
  void forkLauncher (fd_set const *selectedFDs);
  void forkWrite (fd_set const *selectedFDs, int *toCloseFds);
  void handleCGIHeaders ();
  void destroy ();
 public:
  CGIGenerator (Server const &s, Request &r, RequestLine const &rL, const Location &loc, int _fd, int *inpipe, int *outpipe, std::string const &_filePath, std::string const &cgi, ResponseHeader &_resp);
  explicit CGIGenerator (const CGIGenerator &x);
  ~CGIGenerator ();
  CGIGenerator &operator= (const CGIGenerator &x);

  ssize_t Read (char *buf, size_t s, fd_set const *selectedFDS);
  ssize_t getSize (fd_set const *selectedFDs, int *toCloseFds);
  bool fullyHandled () const;
  IGenerator *clone () const;
  std::string genGetMime () const;
};

#endif //_CGIGENERATOR_HPP_
