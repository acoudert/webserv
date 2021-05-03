#include <mimes.hpp>
#include <libft.hpp>
#include "GETResponse.hpp"
#include "pathUtils.hpp"
#include "AutoIndexGenerator.hpp"
#include "EmptyGenerator.hpp"

GETResponse::GETResponse (Server const &s, Request &r, RequestLine const &rL, const Location &loc, Response &_response)
	: server (s), request (r), location (loc), requestLine (rL), generator (NULL), fd (-1), statusCode (r.getStatusCode ()), resp (rL.statusCode), response (_response)
{
  inPipe[0] = -1;
  inPipe[1] = -1;
  outPipe[0] = -1;
  outPipe[1] = -1;
}

GETResponse::GETResponse (GETResponse const &x)
	: server (x.server), request (x.request), location (x.location), requestLine (x.requestLine), generator (NULL), resp (x.resp), response (x.response)
{
  *this = x;
}

GETResponse::~GETResponse ()
{
  if (generator)
	delete generator;
}

GETResponse &GETResponse::operator= (GETResponse const &x)
{
  if (x.generator)
	generator = x.generator->clone ();
  fd = x.fd;
  inPipe[0] = x.inPipe[0];
  inPipe[1] = x.inPipe[1];
  outPipe[0] = x.outPipe[0];
  outPipe[1] = x.outPipe[1];
  statusCode = x.statusCode;
  resp = x.resp;
  return *this;
}

void GETResponse::tryPath (const std::string &path, bool isCGI)
{
  struct stat statbuf;

  if (isCGI)
	goto cgiSkip;
  if (lstat (path.c_str (), &statbuf) < 0)
	return;
  if ((statbuf.st_mode & S_IROTH) == 0)
	{
	  this->fd = -1;
	  this->statusCode = FORBIDDEN;
	  this->generator = this->server.errorGenerator (FORBIDDEN);
	  return;
	}
  if ((fd = open (path.c_str (), O_RDONLY)) < 0)
	return;
  cgiSkip:
  std::string cgi = location.getCGIPath (path);
  if (!cgi.empty ())
	{
	  if (pipe (inPipe) < 0)
		{
		  statusCode = INTERNAL_SERVER_ERROR;
		  generator = this->server.errorGenerator (INTERNAL_SERVER_ERROR);
		  return;
		}
	  if (pipe (outPipe) < 0)
		{
		  statusCode = INTERNAL_SERVER_ERROR;
		  generator = this->server.errorGenerator (INTERNAL_SERVER_ERROR);
		  return;
		}
	  try // syscall failure in cgi throw except to avoid having an error bool
		{
		  if (g_pipe)
			{
			  if (fcntl (inPipe[1], F_SETPIPE_SZ, REQ_BUF_SIZE) < 0)
				std::cout << inPipe[1] << "GET fcntl1: " << strerror (errno)
						  << std::endl;
			  if (fcntl (outPipe[1], F_SETPIPE_SZ, REQ_BUF_SIZE) < 0)
				std::cout << outPipe[1] << "GET fcntl2: " << strerror (errno)
						  << std::endl;
			}
		  generator = new CGIGenerator (server, request, requestLine, location, -1, inPipe, outPipe, path, cgi, resp);
		}
	  catch (std::exception &e)
		{
		  statusCode = INTERNAL_SERVER_ERROR;
		  generator = this->server.errorGenerator (INTERNAL_SERVER_ERROR);
		}
	}
  else
	{
	  this->generator = new FDGenerator (fd, path);
	  char buf[42];
	  struct stat st;
	  struct timeval tv;
	  fstat (fd, &st);
	  tv.tv_sec = st.st_mtim.tv_sec;
	  tv.tv_usec = st.st_mtim.tv_nsec / 1000;
	  tvToStr (buf, &tv);
	  this->resp.set ("Last-Modified", buf);
	}
}

// Define generator to use
void GETResponse::setUp (int *internalFds)
{
  std::string path (server.getFileName (request));

  if (statusCode != OK)
	generator = server.errorGenerator (statusCode);

  struct stat statbuf;
  std::string url = this->request.getUrl ();
  bool isCgi = !this->location.getCGIPath (this->request.getUrl ()).empty ();
  if (isCgi)
	goto cgiSkip;
  if (lstat (path.c_str (), &statbuf) < 0)
	{
	  this->statusCode = getLStatErrorCode ();
	  this->generator = this->server.getError (this->statusCode, internalFds, &this->fd);
	  this->statusCode = this->generator->updateStatusCode (this->statusCode);
	  return;
	}

  this->fd = -2;
  if (S_ISREG (statbuf.st_mode))
	{
	cgiSkip:
	this->tryPath (path, isCgi);
	  if (this->fd >= -1)
		{
		  internalFds[0] = fd; // fd as -1 by default
		  internalFds[1] = inPipe[0]; // fd as -1 by default
		  internalFds[2] = inPipe[1]; // fd as -1 by default
		  internalFds[3] = outPipe[0]; // fd as -1 by default
		  internalFds[4] = outPipe[1]; // fd as -1 by default
		  return;
		}
	}
  else if (S_ISDIR (statbuf.st_mode))
	{
	  if (request.getMethod () != POST)
		{
		  if (url.length () > 1)
			{
			  if (url[url.length () - 1] == '/'
				  && url[url.length () - 2] == '/')
				{
				  while (url.length () != 0 && url[url.length () - 1] == '/')
					{
					  url.erase (--url.end ());
					}
				}
			}
		  if (url[url.length () - 1] != '/')
			{
			  std::string str = "http://" + this->request.get ("Host");
			  str += url;
			  if (str[str.length () - 1] != '/')
				str += '/';
			  this->resp.set ("location", str);
			  this->statusCode = MOVED_PERMANENTLY;
			  this->generator = this->server.errorGenerator (MOVED_PERMANENTLY);
			  return;
			}
		}
	  for (size_t i (0); i < location.getIndexesSize (); ++i)
		{
		  this->tryPath (pathAppend (path, location.getIndex (i)), false);
		  if (this->fd >= -1)
			{
			  internalFds[0] = fd;
			  internalFds[1] = inPipe[0];
			  internalFds[2] = inPipe[1];
			  internalFds[3] = outPipe[0];
			  internalFds[4] = outPipe[1];
			  return;
			}
		}
	  if ((statbuf.st_mode & S_IROTH) == 0)
		{
		  this->statusCode = FORBIDDEN;
		  this->generator = this->server.getError (FORBIDDEN, internalFds, &this->fd);
		  this->statusCode = this->generator->updateStatusCode (this->statusCode);
		  return;
		}
	  if (this->location.getAutoIndex ())
		{
		  if (server.getLocation (request, NULL)->getAutoIndex ())
			{
			  char buf[42];
			  struct timeval tv;
			  tv.tv_sec = statbuf.st_mtim.tv_sec;
			  tv.tv_usec = statbuf.st_mtim.tv_nsec / 1000;
			  tvToStr (buf, &tv);
			  this->resp.set ("Last-Modified", buf);
			  this->generator = new AutoIndexGenerator (path, this->request.getUrl ());
			}
		  else
			{
			  this->statusCode = NOT_FOUND;
			  this->generator = this->server.getError (NOT_FOUND, internalFds, &this->fd);
			  this->statusCode = this->generator->updateStatusCode (this->statusCode);
			}
		  return;
		}
	}
  if (this->statusCode == OK)
	statusCode = NOT_FOUND;
  generator = this->server.getError (statusCode, internalFds, &this->fd); // if error when opening file
  this->statusCode = this->generator->updateStatusCode (this->statusCode);
}

char *
GETResponse::getHeader (size_t *size, fd_set *selectedFDs, int *toCloseFds)
{
  ssize_t s = generator->getSize (selectedFDs, toCloseFds);

  if (s == -1)
	return NULL;
  resp.setCode (statusCode);
  resp.setVersion (request.getVersion ());
  char b[20];
  ft::itoa (b, s);
  this->resp.set ("Content-Length", b);
  std::string mime = this->generator->genGetMime ();
  if (!mime.empty ())
	this->resp.set ("Content-Type", mime);
  *size = this->resp.measure ();
  return this->resp.generate (*size);
}

bool GETResponse::execute (fd_set *selectedFDs, int *toCloseFds)
{
  static size_t hello (0); // Nice idea a static in multithreading, you rock ! :P
  size_t writtenSize;
  size_t bufSize;
  char *buf (NULL);

  bufSize = response.writeAvailable ();
  if (bufSize == 0)
	return false; // May get infinite loop if buffer full
  buf = new char[bufSize];
  writtenSize = generator->Read (buf, bufSize, selectedFDs);
  response.writeBody (buf, writtenSize);
  if (g_verbose && request.getRLine ().method == POST)
	{
	  hello += writtenSize;
	  std::cout << "Sent into writeBuf: " << hello << std::endl;
	  if (writtenSize == 0)
		hello = 0;
	}
  if (generator->fullyHandled ())
	{
	  toCloseFds[0] = fd;
	  toCloseFds[1] = inPipe[0];
	  toCloseFds[2] = inPipe[1];
	  toCloseFds[3] = outPipe[0];
	  toCloseFds[4] = outPipe[1];
	  delete[] buf;
	  return true;
	}
  delete[] buf;
  return false;
}

bool GETResponse::hasInternalFd (int *internalFds) const
{
  internalFds[0] = fd;
  internalFds[1] = inPipe[0];
  internalFds[2] = inPipe[1];
  internalFds[3] = outPipe[0];
  internalFds[4] = outPipe[1];
  return true;
}

IResponse *GETResponse::clone () const
{
  return new GETResponse (*this);
}

unsigned short GETResponse::getLStatErrorCode ()
{
  switch (errno)
	{
	  case EACCES:
		return FORBIDDEN;
	  case ENOENT:
	  case ENAMETOOLONG:
		return NOT_FOUND;
	  default:
		return INTERNAL_SERVER_ERROR;
	}
}

void GETResponse::headerSet (const std::string &key, const std::string &value)
{
  this->resp.set (key, value);
}

void GETResponse::destroy ()
{
  if (this->inPipe[0] != -1)
	close (this->inPipe[0]);
  if (this->inPipe[1] != -1)
	close (this->inPipe[1]);
  if (this->outPipe[0] != -1)
	close (this->outPipe[0]);
  if (this->outPipe[1] != -1)
	close (this->outPipe[1]);
  if (this->generator != NULL)
	this->generator->destroy ();
}
