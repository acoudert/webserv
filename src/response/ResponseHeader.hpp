#ifndef _RESPONSEHEADER_HPP_
#define _RESPONSEHEADER_HPP_

#include "StatusCode.hpp"
#include "utils.hpp"
#include <map>

class ResponseHeader {
 private:
  Versions version;
  short code;
  std::string message;
  std::map<std::string, std::string> fields;

  static StatusCodes const codeHelper[STATUSCODE_NB];

  ResponseHeader ();
 public:
  explicit ResponseHeader (short statusCode);
  ResponseHeader (const ResponseHeader &x);
  ResponseHeader &operator= (const ResponseHeader &x);
  size_t measure () const;

  // return a new[] allocated pointer
  // s must be bigger or equal to the output size (you must call measure() to
  // know that
  char *generate (size_t s);

  unsigned short getCode () const;
  void setVersion (Versions v);
  void setCode (short code);
  void setMessage (const std::string &message);
  void set (const std::string &key, const std::string &value);
};

#endif //_RESPONSEHEADER_HPP_

