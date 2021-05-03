#include <libft.hpp>
#include "Request.hpp"

// Request Class Definitions
Request::Request (ServerGroup const &_servers)
	: port (0), headersReady (false), queryStr (""), statusCode (OK), _alive (true), _fullyParsed (false), readBuf (REQ_BUF_SIZE), chunkSize (-1), sizeDataReceived (-1), servers (_servers), server (NULL), loc (NULL)
{
  gettimeofday (&last, NULL);
}

Request::Request (Request const &src)
	: port (0), headersReady (false), queryStr (""), statusCode (OK), _alive (true), _fullyParsed (false), readBuf (REQ_BUF_SIZE), chunkSize (-1), servers (src.servers), server (NULL), loc (NULL)
{
  *this = src;
}

Request &Request::operator= (Request const &src)
{
  this->rLine = src.rLine;
  this->headers = src.headers;
  this->port = src.port;
  this->headersReady = src.headersReady;
  this->queryStr = src.queryStr;
  this->body = src.body;
  this->statusCode = src.statusCode;
  this->_alive = src._alive;
  this->_fullyParsed = src._fullyParsed;
  this->readBuf = src.readBuf;
  this->chunkSize = src.chunkSize;
  this->sizeDataReceived = src.sizeDataReceived;
  this->firstTime.tv_sec = src.firstTime.tv_sec;
  this->last.tv_sec = src.last.tv_sec;
  this->cur.tv_sec = src.cur.tv_sec;
  if (src.server)
	this->server = new Server (*src.server);
  if (src.loc)
	this->loc = new Location (*src.loc);
  this->port = src.port;
  return *this;
}

Request::~Request ()
{
  if (server)
	delete server;
  if (loc)
	delete loc;
}

void Request::displayRequest () const
{
  std::cout << "REQUEST" << std::endl;
  std::cout << "Status Code: " << statusCode << std::endl;
  std::cout << "Port: " << port << std::endl;
  std::cout << "Fully Parsed: " << std::boolalpha << _fullyParsed
			<< std::noboolalpha << std::endl;
  std::cout << "Alive: " << std::boolalpha << _alive << std::noboolalpha
			<< std::endl;
  std::cout << "Request Line:" << std::endl;
  rLine.displayRequestLine ();
  std::cout << "Headers:" << std::endl;
  for (std::map<std::string, std::string>::const_iterator it (headers.begin ());
	   it != headers.end (); ++it)
	{
	  std::cout << "\t" << it->first << ":" << it->second << std::endl;
	}
  std::cout << "Query String: " << queryStr << std::endl;
  std::cout << "Body:" << std::endl << "\t";
  // for (size_t i (0); i < body.size (); i++)
  //std::cout << body[i];
  std::cout << std::endl;
}

bool Request::alive () const
{
  return this->_alive;
}

void Request::setAlive (bool const &alive)
{
  _alive = alive;
}

bool Request::fullyParsed () const
{
  return this->_fullyParsed;
}

void Request::onReadable (int const &fd, fd_set const *selectedFDs)
{
  gettimeofday (&cur, NULL);
  if (g_timeout && cur.tv_sec - last.tv_sec > 30) // internal timeout timer
	this->_alive = false;
  if (FD_ISSET(fd, &selectedFDs[0]))
	{
	  char buf[this->readBuf.available () + 1];
	  ssize_t n = recv (fd, buf, this->readBuf.available (), 0);
	  // IF FD_ISSET(read) + RECV == 0 => CLIENT CLOSED CONNECTION
	  if (n <= 0)
		this->_alive = false;
	  else if (n > 0)
		{
		  this->readBuf.push (buf, n);
		  last.tv_sec = cur.tv_sec;
		  if (sizeDataReceived == -1) // if we are receiving first+ byte
			{
			  sizeDataReceived = 0;
			  firstTime.tv_sec = cur.tv_sec;
			}
		}
	}
  if (!this->fullyParsed () && this->readBuf.used () >= 1)
	{
	  if (!parseRequest ())
		{
		  readBuf.pop (readBuf.used ()); // empty buffer if request error
		  this->_fullyParsed = true;
		  return; // Error request (refer to nginx behavior), errCode in rLine.statusCode
		}
	  if (g_verbose && (rLine.method == POST || rLine.method == PUT))
		{
		  std::cout << "Request body.size(): " << body.size () << std::endl;
		}
	}
}

size_t Request::readBody (char *buf, size_t bufSize)
{
  size_t s = std::min (bufSize, this->readBuf.used ());
  ft::memcpy (buf, this->readBuf.get (), s);
  this->readBuf.pop (s);
  return s;
}

int Request::getStatusCode () const
{
  return statusCode;
}

std::string Request::get (const std::string &key) const
{
  std::map<std::string, std::string>::const_iterator it = this->headers.find (key);
  if (it == this->headers.end ())
	return "";
  return it->second;
}

std::map<std::string, std::string> const &Request::getHeaders () const
{
  return headers;
}

void Request::setPort (t_port const &_port)
{
  port = _port;
}

t_port const &Request::getPort () const
{
  return port;
}

Methods Request::getMethod () const
{
  return this->rLine.method;
}

std::string Request::getUrl () const
{
  return this->rLine.target;
}

Versions Request::getVersion () const
{
  if (this->rLine.httpVersion == "HTTP/1.0")
	return HTTP_1_0;
  else if (this->rLine.httpVersion == "HTTP/1.1")
	return HTTP_1_1;
  return UNKNOWN_VERSION;
}

std::string const Request::getQueryStr () const
{
  return queryStr;
}

std::vector<char> &Request::getBody ()
{
  return body;
}

RequestLine const &Request::getRLine () const
{
  return rLine;
}

Server const *Request::getServer () const
{
  return server;
}

Location const *Request::getLocation () const
{
  return loc;
}

Buffer const &Request::getReadBuf () const
{
  return readBuf;
}

void Request::setLast ()
{
  gettimeofday (&last, NULL);
}

// Return true if request to remove from worker
bool Request::cleanInternalData ()
{
  rLine.method = UNKNOWN_METHOD;
  rLine.target = "";
  rLine.httpVersion = "";
  rLine.statusCode = OK;
  rLine.ready = false;
  headers.clear ();
  headersReady = false;
  queryStr.clear ();
  body.clear ();
  statusCode = OK;
  sizeDataReceived = -1;
  _fullyParsed = false;
  chunkSize = -1;
  if (server)
	{
	  delete server;
	  server = NULL;
	}
  if (loc)
	{
	  delete loc;
	  loc = NULL;
	}
  return false;
}

bool Request::slowloris ()
{
  if (g_slowloris)
	{
	  // Based on 2G lowest stream divided by 2
	  ssize_t nbBytesPerSec =
		  100 * (-1 / (last.tv_sec - firstTime.tv_sec + 1) + 1);
	  // Case slowloris attack
	  if (sizeDataReceived + readBuf.used ()
		  < size_t ((last.tv_sec - firstTime.tv_sec + 1) * nbBytesPerSec))
		{
		  std::cout << "Slowloris detected, dropping request." << std::endl;
		  _alive = false;
		  return true;
		}
	}
  return false;
}

//bool Request::parseRequest (const std::string &content)
bool Request::parseRequest ()
{
  size_t maxBody (0);
  size_t index (0);
  std::string line;

  if (!rLine.ready) // Parse requestLine
	{
	  char const *tempRL = this->readBuf.get ();
	  index = 0;
	  while (tempRL && index + 1 < this->readBuf.used ()
			 && tempRL[index] != '\r' && tempRL[index + 1] != '\n')
		index++;
	  if (slowloris ())
		return false; // pop buffer
	  if (tempRL[index] != '\r' || tempRL[index + 1] != '\n')
		{
		  return true;
		}
	  if (tempRL[index] == '\r' && tempRL[index + 1] == '\n')
		{
		  rLine = RequestLine (std::string (this->readBuf.get (), index));
		  if (rLine.statusCode != OK)
			{
			  statusCode = rLine.statusCode;
			  return false; // error in requestLine => Send error directly, refer to nginx
			}
		  sizeDataReceived = index + 2;
		  this->readBuf.pop (index + 2);
		  parseQueryStr ();
		  rLine.ready = true;
		}
	}
  while (rLine.ready && !headersReady) // parse headers
	{
	  char const *tempH = this->readBuf.get ();
	  index = 0;
	  while (tempH && index + 1 < this->readBuf.used ()
			 && tempH[index] != '\r' && tempH[index + 1] != '\n')
		index++;
	  if (slowloris ())
		return false;
	  if (tempH[index] != '\r' || tempH[index + 1] != '\n') // No CRLF
		{
		  if (index > MAXHEADER_LEN)
			{
			  statusCode = BAD_REQUEST;
			  return false;
			}
		  return true;
		}
	  line = std::string (this->readBuf.get (), index);
	  sizeDataReceived += index + 2;
	  this->readBuf.pop (index + 2);
	  if (line.length ()
		  > MAXHEADER_LEN) // error if header too long sent at once
		{
		  statusCode = BAD_REQUEST;
		  return false;
		}
	  if (line.length () != 0)
		parseHeader (line);
	  else
		{
		  if (headers.find ("Content-Length") != headers.end ())
			body.reserve (static_cast<size_t>(ft::atoi (headers.find ("Content-Length")->second.c_str ())));
		  headersReady = true;
		  const Server *serv = servers.getServer (*this);
		  if (serv != NULL)
			{
			  server = new Server (*serv);
			  if (server && server->getLocation (*this, NULL))
				loc = new Location (*server->getLocation (*this, NULL));
			  else
				statusCode = NOT_ALLOWED;
			}
		  else
			statusCode = 418;
		}
	}
  // Already checked that there is a location to extract size from
  if (server && loc && rLine.ready && headersReady && !_fullyParsed
	  && (rLine.method == POST || rLine.method == PUT))
	maxBody = server->getLocation (*this, NULL)->getMaxBodySize ();
  while (headersReady)
	{
	  bool allBodyReceived = false;
	  if (headers.find ("Content-Length") == headers.end ()
		  && headers.find ("Transfer-Encoding") != headers.end ()
		  && (headers.find ("Transfer-Encoding")->second == "chunked"
			  || headers.find ("Transfer-Encoding")->second == "Chunked"))
		{
		  if (!parseChunk (allBodyReceived, maxBody))
			return false;
		  if (!allBodyReceived)
			return true;
		}
	  else if (headers.find ("Content-Length") != headers.end ())
		{
		  if (!getBody (allBodyReceived, maxBody))
			return false;
		  if (!allBodyReceived)
			return true;
		}
	  this->_fullyParsed = true;
	  if (g_vverbose)
		displayRequest ();
	  return true;
	}
  return true;
}

void Request::parseQueryStr ()
{
  std::string fullParameters;
  std::string key;
  std::string value;

  if (rLine.target.find ("?") == std::string::npos)
	return;
  queryStr = std::string (rLine.target, rLine.target.find ("?") + 1);
  rLine.target.erase (rLine.target.find ("?"));
}

void Request::parseHeader (const std::string &line)
{
  size_t index (findIndexChar (line, ":"));

  std::string field (line, 0, index);
  std::string value;
  if (index != line.length ())
	{
	  if (line[index + 1] == ' ')
		value = std::string (line, index + 2); // + 2 to jump ': '
	  else
		value = std::string (line, index + 1); // + 2 to jump ': '
	}

  headers.insert (std::pair<std::string, std::string> (field, value));
}

bool Request::parseChunk (bool &allBodyReceived, size_t const &maxBody)
{
  size_t i (0);

  if (slowloris ())
	return false;
  if (chunkSize == -1) // Beginning of single chunk
	{
	  char const *tempSize = readBuf.get ();
	  std::stringstream strStream;

	  if (readBuf.used () < 3) // min length for size declaration of 0
		{
		  return true;
		}
	  while (tempSize && i + 1 < readBuf.used ()
			 && tempSize[i] != '\r' && tempSize[i + 1] != '\n')
		++i;
	  for (size_t j (0); j < i; ++j) // check hex number
		{
		  if ((tempSize[j] >= '0' && tempSize[j] <= '9')
			  || (tempSize[j] >= 'a' && tempSize[j] <= 'f')
			  || (tempSize[j] >= 'A' && tempSize[j] <= 'F'))
			continue;
		  else
			{
			  statusCode = BAD_REQUEST;
			  return false;
			}
		}
	  if (tempSize[i] != '\r' || tempSize[i + 1] != '\n')
		{
		  return true;
		}
	  strStream << std::hex << std::string (tempSize, i);
	  strStream >> chunkSize; // put size of chunk into request var
	  sizeDataReceived += i + 2;
	  readBuf.pop (i + 2); // Pop line with size, CRLF included
	}
  // Case of last chunk
  if (chunkSize == 0)
	{
	  char const *tempLast = readBuf.get ();

	  if (readBuf.used () < 2)
		{
		  return true;
		}
	  else if (tempLast[0] == '\r' && tempLast[1] == '\n')
		{
		  sizeDataReceived += 2;
		  readBuf.pop (2); // pop last line
		  allBodyReceived = true; // End of parsing
		  return true; // No error
		}
	  else // case last chunk malformed
		{
		  statusCode = BAD_REQUEST;
		  return false;
		}
	}
  if (body.size () + chunkSize > maxBody)
	{
	  statusCode = PAYLOAD_TOO_LARGE; // Body too large
	  //return true;
	}
  if (statusCode == OK && body.size () + chunkSize
						  < body.capacity ())    // vector cannot hold body
	{
	  if (body.size () == 0)
		body.reserve (2);
	  while (body.capacity () < body.size () + chunkSize)
		body.reserve (body.size () * 2);
	}
  while (chunkSize != 0)
	{
	  if (readBuf.used () == 0)
		return true; // All chunk not received with current buf
	  if (chunkSize == 1
		  && readBuf.used () < 3) // case we dont have CRLF in buf
		{
		  return true;
		}
	  if (statusCode == OK && (rLine.method == PUT || rLine.method == POST))
		body.insert (body.end (), readBuf.get ()[0]);
	  sizeDataReceived += 1;
	  readBuf.pop (1);
	  --chunkSize;
	  if (chunkSize == 0)
		{
		  chunkSize = -1;
		  if (readBuf.get ()[0] != '\r' || readBuf.get ()[1] != '\n')
			{
			  statusCode = BAD_REQUEST;
			  return false;
			}
		  sizeDataReceived += 2;
		  readBuf.pop (2); // pop final CRLF (check presence from chunkSize == 1 condition)
		  return true;
		}
	}
  return true;
}

bool Request::getBody (bool &allBodyReceived, size_t const &maxBody)
{
  if (slowloris ())
	return false;
  if (chunkSize == -1) // Beginning of single chunk
	{
	  chunkSize = static_cast<ssize_t>(ft::atoi (headers.find ("Content-Length")->second.c_str ())); // if vector reserve gave more than expected
	}
  if (chunkSize + body.size () > maxBody)
	{
	  statusCode = PAYLOAD_TOO_LARGE;
	}
  else
	{
	  if (body.size () == 0)
		body.reserve (2);
	  while (body.capacity () < body.size () + chunkSize)
		body.reserve (body.size () * 2);
	}
  while (chunkSize != 0)
	{
	  if (this->readBuf.used () == 0)
		{
		  return true;
		}
	  if (statusCode == OK && (rLine.method == PUT || rLine.method == POST))
		body.insert (body.end (), this->readBuf.get ()[0]);
	  sizeDataReceived += 1;
	  this->readBuf.pop (1);
	  chunkSize -= 1;
	}
  chunkSize = -1;
  allBodyReceived = true; // check with NGINX if error if body > content-length
  return true;
}
