#ifndef _LOCATION_HPP_
#define _LOCATION_HPP_

#define CANARY 0x42424242

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include "Methods.hpp"
class Location;
#include "Request.hpp"
#include "Server.hpp"
#include "CGI.hpp"

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>

#define DEFAULT_MAXBODYSIZE    SIZE_MAX

#define ON    1
#define OFF    0
#define LOCATIONSUBBLOCK_NB    7

typedef bool    (Location::*functionLocation) (std::ifstream &file, std::string &line);

struct LocationSubBlock {
	std::string name;
	functionLocation ft;
};

class Location {
 private:
  std::string location;
  std::string root;
  std::vector<std::string> indexes;
  std::vector<Methods> acceptedMethods;
  bool autoIndex; // true = ON | false = OFF
  std::vector<CGI> CGIs;
  std::string saveDir;
  size_t maxBodySize;
  bool error;

 public:
  Location ();
  Location (std::string const &location, std::string const &root);
  Location (std::ifstream &file, std::string &line);
  Location (const Location &x);
  ~Location ();
  Location &operator= (const Location &x);

  void displayLocation () const;
  bool errorConf () const;
  const std::string &getSaveDir () const;

  std::string const &getLocation () const;
  std::string const &getRoot () const;
  std::string const &getIndex (size_t const &i) const;
  size_t getIndexesSize () const;
  Methods const &getAcceptedMethod (size_t const &i) const;
  size_t getAcceptedMethodsSize () const;
  bool const &getAutoIndex () const;
  CGI const &getCGI (size_t const &i) const;
  size_t getCGIsSize () const;
  size_t getMaxBodySize () const;
  std::string getCGIPath (std::string const &filePath) const;

  unsigned long long int
  matches (const Request &r, std::map<Methods, bool> *meths) const;

//  bool matchUrl (const std::string &url);

  bool extractLocation (std::ifstream &file, std::string &line);
  bool rootSubBlock (std::ifstream &file, std::string &line);
  bool indexSubBlock (std::ifstream &file, std::string &line);
  bool methodsSubBlock (std::ifstream &file, std::string &line);
  bool autoIndexSubBlock (std::ifstream &file, std::string &line);
  bool cgiSubBlock (std::ifstream &file, std::string &line);
  bool saveDirSubBlock (std::ifstream &file, std::string &line);
  bool clientMaxBodySizeSubBlock (std::ifstream &file, std::string &line);
  std::string getFileName (const Request &r) const;
};

#endif //_LOCATION_HPP_
