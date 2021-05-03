#ifndef _HEADRESPONSE_HPP_
#define _HEADRESPONSE_HPP_

#include "GETResponse.hpp"

class HEADResponse : public GETResponse {
 private:
  HEADResponse ();
 public:
  HEADResponse (Server const &s, Request &r, RequestLine const &rL, const Location &loc, Response &response);
  HEADResponse (const HEADResponse &x);
  HEADResponse &operator= (const HEADResponse &x);

  bool execute (fd_set *selectedFDs, int *toCloseFds);

  IResponse *clone () const;
};

#endif //_HEADRESPONSE_HPP_
