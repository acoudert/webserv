#include <libft.hpp>
#include "PUTResponse.hpp"

PUTResponse *PUTResponse::clone () const
{
  return new PUTResponse (*this);
}

PUTResponse::~PUTResponse ()
{
  if (this->outFd > 0)
	close (this->outFd);
  delete this->gen;
}

PUTResponse::PUTResponse (Response &resp, const Location &loc)
	: resp (resp), outFd (-1), loc (&loc), executed (false), headers (200), copyOffset (0), gen (NULL)
{}

PUTResponse::PUTResponse (const PUTResponse &x)
	: resp (x.resp), outFd (x.outFd), loc (x.loc), executed (x.executed), headers (x.headers), copyOffset (x.copyOffset), gen (NULL)
{
  if (x.gen != NULL)
	this->gen = x.gen->clone ();
}

PUTResponse &PUTResponse::operator= (const PUTResponse &x)
{
  this->resp = x.resp;
  this->outFd = x.outFd;
  this->loc = x.loc;
  this->executed = x.executed;
  this->headers = x.headers;
  this->copyOffset = x.copyOffset;
  delete this->gen;
  if (x.gen != NULL)
	this->gen = x.gen->clone ();
  return *this;
}

void PUTResponse::setUp (int *internalFds)
{
  std::string dir = this->loc->getSaveDir ();
  std::string p = this->resp.getRequest ().getUrl ();
  if (p[p.length () - 1] == '/')
	p.erase (p.length () - 1, 1);
  if (dir[dir.length () - 1] != '/')
	dir += '/';
  p.erase (0, this->loc->getLocation ().length () + 1);

  while (p.find ('/') != std::string::npos)
	{
	  size_t i;
	  for (i = 0; i < p.length () && p[i] != '/'; ++i)
		dir += p[i];
	  p.erase (0, i == p.length () ? i : i + 1);
	  if (mkdir (dir.c_str (), S_IRWXU | S_IRWXG | S_IRWXO) < 0)
		{
		  if (errno == EEXIST)
			continue;
		  this->headers.setCode (INTERNAL_SERVER_ERROR);
		}
	}
  struct stat st;
  if (lstat (dir.c_str (), &st) < 0)
	{
	  this->headers.setCode (201);
	}
  dir += p;
  this->outFd = open (dir.c_str (),
					  O_WRONLY | O_CREAT | O_TRUNC,
					  S_IRWXU | S_IRWXG | S_IRWXO);
  if (this->outFd < 0)
	{
	  this->headers.setCode (INTERNAL_SERVER_ERROR);
	}
  internalFds[0] = this->outFd;
}

char *
PUTResponse::getHeader (size_t *size, fd_set *selectedFDs, int *toCloseFds)
{
  if (this->executed
	  || (this->headers.getCode () != OK && this->headers.getCode () != 201))
	{
	  if (this->gen == NULL)
		this->gen = new ErrorGenerator (this->headers.getCode ());
	  ssize_t s = this->gen->getSize (selectedFDs, toCloseFds);

	  if (s == -1)
		return NULL; // check lstat size in setUp or infinite loop if file.size == 0
	  this->headers.setVersion (this->resp.getRequest ().getVersion ());
	  char b[20];
	  ft::itoa (b, s);
	  this->headers.set ("Content-Length", b);
	  *size = this->headers.measure ();
	  return this->headers.generate (*size);
	}
  else
	{
	  this->execute (selectedFDs, toCloseFds);
	  if (this->outFd != -1)
		FD_CLR(this->outFd, &selectedFDs[1]);
	}
  return NULL;
}

void PUTResponse::headerSet (const std::string &key, const std::string &value)
{
  this->headers.set (key, value);
}

bool PUTResponse::execute (fd_set *selectedFDs, int *toCloseFds)
{
  if (this->executed)
	{
	  size_t writtenSize;
	  size_t bufSize;

	  bufSize = this->resp.writeAvailable ();
	  if (bufSize == 0)
		return false; // May get infinite loop if buffer full
	  char buf[bufSize];
	  writtenSize = this->gen->Read (buf, bufSize, selectedFDs);
	  this->resp.writeBody (buf, writtenSize);
	  return this->gen->fullyHandled ();
	}
  if (FD_ISSET(this->outFd, &selectedFDs[1]))
	{
	  ssize_t ret;
	  if (!this->resp.getRequest ().getBody ().empty ())
		ret = write (this->outFd, &this->resp.getRequest ().getBody ()[this->copyOffset],
					 this->resp.getRequest ().getBody ().size ()
					 - this->copyOffset);
	  else
		ret = 0;
	  if (ret < 0)
		{
		  this->headers.setCode (INTERNAL_SERVER_ERROR);
		  this->gen = new ErrorGenerator (INTERNAL_SERVER_ERROR);
		}
	  else
		this->copyOffset += ret;
	  if (this->copyOffset == this->resp.getRequest ().getBody ().size ()
		  || ret <= 0)
		{
		  close (this->outFd);
		  toCloseFds[0] = this->outFd;
		  this->outFd = -1;
		  this->executed = true;
		  if (this->gen == NULL)
			this->gen = new ErrorGenerator (this->headers.getCode ());
		}
	}
  return false;
}

bool PUTResponse::hasInternalFd (int *internalFds) const
{
  internalFds[0] = this->outFd;
  return true;
}

void PUTResponse::destroy ()
{
  if (this->gen != NULL)
	this->gen->destroy ();
}
