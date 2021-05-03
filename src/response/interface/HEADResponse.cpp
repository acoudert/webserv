#include "HEADResponse.hpp"

IResponse *HEADResponse::clone () const
{
  return new HEADResponse (*this);
}

HEADResponse::HEADResponse (const HEADResponse &x)
	: GETResponse (x)
{}

HEADResponse &HEADResponse::operator= (const HEADResponse &x)
{
  GETResponse::operator= (x);
  return *this;
}

bool HEADResponse::execute (fd_set *selectedFDs, int *toCloseFds)
{
  (void) selectedFDs;
  this->hasInternalFd (toCloseFds);
  return true;
}

HEADResponse::HEADResponse (const Server &s, Request &r, const RequestLine &rL, const Location &loc, Response &response)
	: GETResponse (s, r, rL, loc, response)
{}

