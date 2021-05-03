#ifndef _REQUEST_HPP_
#define _REQUEST_HPP_

#include <sys/socket.h>
#include <sys/time.h>
#include <cstring>
#include <cassert>
#include <map>
#include <list>
#include <string>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include "Global.hpp"
#include "Methods.hpp"
#include "utils.hpp"
#include "Buffer.hpp"
#include "RequestLine.hpp"
class Request;
class ServerGroup;
#include "ServerGroup.hpp"
#include "Server.hpp"
#include "Location.hpp"

class Request {
 public:
  Request (ServerGroup const &_servers);
  Request (Request const &src);
  Request &operator= (Request const &src);
  ~Request ();
  void displayRequest () const;

  bool alive () const;
  void setAlive (bool const &alive);
  bool fullyParsed () const;

  void onReadable (int const &fd, fd_set const *selectedFDs);
  size_t readBody (char *buf, size_t bufSize);

  int getStatusCode () const;
  std::string get (const std::string &key) const;
  std::map<std::string, std::string> const &getHeaders () const;
  void setPort (t_port const &_port);
  t_port const &getPort () const;
  Methods getMethod () const;
  std::string getUrl () const;
  Versions getVersion () const;
  std::string const getQueryStr () const;
  std::vector<char> &getBody ();
  RequestLine const &getRLine () const;
  Server const *getServer () const;
  Location const *getLocation () const;
  Buffer const &getReadBuf () const;
  void setLast ();

  bool cleanInternalData ();

 private:
  Request ();

  RequestLine rLine;
  std::map<std::string, std::string> headers;
  t_port port;
  bool headersReady;
  std::string queryStr;
  std::vector<char> body;
  int statusCode;

  bool _alive; // False = Connection to close
  bool _fullyParsed;

  // Response Buffers
  Buffer readBuf;
  ssize_t chunkSize;
  ssize_t sizeDataReceived; // for slowloris

  // Timer
  struct timeval firstTime; // for slowloris
  struct timeval last;
  struct timeval cur;

  // ServerGroup

  ServerGroup const &servers;
  Server const *server;
  Location const *loc;

  bool slowloris ();
  //bool parseRequest (const std::string &content); // return false if error
  bool parseRequest (); // return false if error
  void parseQueryStr ();
  void parseHeader (const std::string &line);
  bool parseChunk (bool &allBodyReceived, size_t const &maxBody);
  bool getBody (bool &allBodyReceived, size_t const &maxBody);
};

#endif // _REQUEST_HPP_
