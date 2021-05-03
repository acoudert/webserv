#include <libft.hpp>
#include "ErrorResponse.hpp"

void ErrorResponse::setUp (int *internalFds)
{
  (void) internalFds;
  this->gen = new ErrorGenerator (this->code);
}

char *
ErrorResponse::getHeader (size_t *size, fd_set *selectedFDs, int *toCloseFds)
{
  (void) selectedFDs;
  (void) toCloseFds;
  char b[20];
  ft::itoa (b, this->gen->getSize (selectedFDs, toCloseFds));
  this->header.set ("Content-Length", b);
  this->header.setCode (this->code);
  *size = this->header.measure ();
  return this->header.generate (*size);
}

bool ErrorResponse::execute (fd_set *selectedFDs, int *toCloseFds)
{
  size_t writtenSize;
  size_t bufSize;
  char *buf (NULL);

  (void) toCloseFds;
  if (this->r.getRequest ().getRLine ().method == HEAD)
	return true;

  bufSize = this->r.writeAvailable ();
  if (bufSize == 0)
	return false; // May get infinite loop if buffer full
  buf = new char[bufSize];
  writtenSize = this->gen->Read (buf, bufSize, selectedFDs);
  if (writtenSize == 0)
	{
	  delete[] buf;
	  return true;
	}
  this->r.writeBody (buf, writtenSize);
  delete[] buf;
  return false;
}

IResponse *ErrorResponse::clone () const
{
  return new ErrorResponse (*this);
}

ErrorResponse::~ErrorResponse ()
{}

ErrorResponse::ErrorResponse (const ErrorResponse &x)
	: code (x.code), r (x.r), gen (NULL), header (x.header)
{}

ErrorResponse &ErrorResponse::operator= (const ErrorResponse &x)
{
  this->code = x.code;
  this->gen = x.gen->clone ();
  this->header = x.header;
  return *this;
}
ErrorResponse::ErrorResponse (Response &r, unsigned short code)
	: code (code), r (r), gen (NULL), header (code)
{}

bool ErrorResponse::hasInternalFd (int *internalFds) const
{
  (void) internalFds;
  return false;
}

void ErrorResponse::headerSet (const std::string &key, const std::string &value)
{
  this->header.set (key, value);
}

void ErrorResponse::destroy ()
{
  delete this->gen;
}
