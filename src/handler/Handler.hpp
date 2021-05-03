#ifndef _HANDLER_HPP_
#define _HANDLER_HPP_

#include <fcntl.h>
#include <cstring>
#include <list>
#include "Request.hpp"
#include "Response.hpp"

class Response;

#define REQ_TIMEOUT 60

struct Connection {
  Connection (int const &_fd, Request const &r, t_port const &port)
	  : fd (_fd), resp (Response (r))
  {
	resp.getRequest ().setPort (port);
  }
  Connection (Connection const &src)
	  : fd (src.fd), resp (src.resp)
  {}
  int fd;
  Response resp;
 private:
  Connection ();
};

class Handler {
 private:
  std::list<Connection> listCon;

  std::list<Connection>::iterator findConnectionIt (int const &fd);
 public:
  explicit Handler ();
  ~Handler ();
  void AddRequest (Request &r, int fd, t_port const &port);
  bool
  signal (int const &fd, fd_set *selectedFDs, int *toAddFds, int *toCloseFds);
  bool
  handleConnection (Connection &c, fd_set *selectedFDs, int *toAddFds, int *toCloseFds);
  bool shouldWriteSelect (int fd);
  void eraseRequest (int const &fd);
  bool cleanData (int const &fd);
};

#endif //_REQUESTHANDLER_HPP_
