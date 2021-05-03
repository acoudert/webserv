#ifndef _REQUESTLINE_HPP_
#define _REQUESTLINE_HPP_

#include <string>
#include <iostream>
#include "Methods.hpp"
#include "StatusCode.hpp"

struct RequestLine {
  Methods method;
  std::string target;
  std::string httpVersion;
  int statusCode;
  bool ready;

  RequestLine ();
  explicit RequestLine (std::string const &str);
  RequestLine (RequestLine const &src);
  ~RequestLine ();
  RequestLine &operator= (RequestLine const &src);
  void displayRequestLine () const;

 private:
  void extractHTTPVersion (std::string &line);
  void extractMethod (std::string &line);
  void extractRequestTarget (std::string &line);
};

#endif // _REQUESTLINE_HPP_
