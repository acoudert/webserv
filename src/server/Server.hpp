#ifndef _SERVER_HPP_
#define _SERVER_HPP_

#include <vector>
#include <map>
#include <string>
#include <sys/types.h>
#include <netinet/in.h>
#include <iostream>
//#include <tcpd.h>
#include "IGenerator.hpp"
#include "ErrorGenerator.hpp"
class Server;
#include "Request.hpp"
#include "Location.hpp"

#define SERVERSUBBLOCK_NB       6
#define FALSE 0
#define TRUE 1
#define POISONING 2

class Server {
 private:
  in_port_t listenPort;
  std::string root;
  std::vector<std::string> serverNames;
  std::vector<std::string> indexes;
  std::map<unsigned short, std::string> errorPages;
  std::vector<Location> locations;
  bool error;

  void addRootLocation ();
 public:
  Server ();
  Server (std::ifstream &file, std::string &line);
  Server (const Server &x);
  ~Server ();
  Server &operator= (const Server &x);

  bool matches (const Request &r) const;
  int canBeDefault (const Request &r) const;

  void displayServer () const;
  bool errorConf () const;

  in_port_t const &getListenPort () const;
  std::string const &getRoot () const;
  std::string const &getServerName (size_t const &i) const;
  size_t getServerNamesSize () const;
  std::string const &getIndex (size_t const &i) const;
  size_t getIndexesSize () const;
  std::string const &getErrorPage (unsigned short const &errorCode) const;
  std::map<unsigned short, std::string> const &getErrorPagesMap () const;
  size_t getErrorPagesSize () const;
  Location const &getLocation (size_t const &i) const;
  size_t getLocationsSize () const;

  IGenerator *getError (unsigned short code, int *toAddFds, int *newFd) const;

  bool extractServer (std::ifstream &file, std::string &line);
  bool listenSubBlock (std::ifstream &file, std::string &line);
  bool rootSubBlock (std::ifstream &file, std::string &line);
  bool indexSubBlock (std::ifstream &file, std::string &line);
  bool serverNameSubBlock (std::ifstream &file, std::string &line);
  bool errorPageSubBlock (std::ifstream &file, std::string &line);
  bool locationSubBlock (std::ifstream &file, std::string &line);

  IGenerator *errorGenerator (unsigned short code) const;

  Location const *
  getLocation (const Request &r, std::map<Methods, bool> *meths) const;
  std::string getFileName (const Request &req) const;
};

typedef bool    (Server::*functionServer) (std::ifstream &file, std::string &line);

struct ServerSubBlock {
	std::string name;
	functionServer ft;
};

#endif //_SERVER_HPP_
