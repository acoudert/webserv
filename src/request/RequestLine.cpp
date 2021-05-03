#include "RequestLine.hpp"
#include "pathUtils.hpp"

// Request Line Struct Definitions
RequestLine::RequestLine ()
	: method (UNKNOWN_METHOD), target (""), httpVersion (""), statusCode (OK), ready (false)
{}

RequestLine::RequestLine (std::string const &str)
	: method (UNKNOWN_METHOD), target (""), httpVersion (""), statusCode (OK), ready (false)
{
  std::string line (str);

  extractHTTPVersion (line); // HTTP/1.0 handles several spaces, HTTP/1.1 doesnt
  if (statusCode != OK)
	return;
  extractMethod (line);
  if (statusCode != OK)
	return;
  extractRequestTarget (line);
  if (line.length () != 0)
	statusCode = BAD_REQUEST;
}

RequestLine::RequestLine (RequestLine const &src)
{
  *this = src;
}

RequestLine::~RequestLine ()
{}

RequestLine &RequestLine::operator= (RequestLine const &src)
{
  method = src.method;
  target = src.target;
  httpVersion = src.httpVersion;
  statusCode = src.statusCode;
  ready = src.ready;
  return *this;
}

void RequestLine::displayRequestLine () const
{
  std::cout << "\tMethod : [" << method << "] " << strMethods[method]
			<< std::endl;
  std::cout << "\tTarget : " << target << std::endl;
  std::cout << "\tVersion: " << httpVersion << std::endl;
  std::cout << "\tCode   : " << statusCode << std::endl;
}

// Setup httpVersion value + update statusCode if required
void RequestLine::extractHTTPVersion (std::string &line)
{
  size_t index (line.rfind (" "));

  if (line.find (" ") == std::string::npos)
	{
	  statusCode = BAD_REQUEST;
	  return;
	}
  httpVersion = std::string (line, index + 1);
  line.erase (index + 1); // Keep trailing space
  if (httpVersion != "HTTP/1.0" && httpVersion != "HTTP/1.1")
	statusCode = 505;
}

// Setup method enum value + update statusCode if required
void RequestLine::extractMethod (std::string &line)
{
  int index (line.find (" "));

  if (line.find (" ") == std::string::npos)
	{
	  statusCode = BAD_REQUEST;
	  return;
	}
  std::string mStr (line, 0, index);
  while (httpVersion == "HTTP/1.0" && line[index] == ' ')
	index++; // To remove all spaces if HTTP/1.0
  if (httpVersion == "HTTP/1.1")
	index++;
  line.erase (0, index);
  for (int i (0); i < METHODS_NB - 1; i++)
	if (mStr == strMethods[i])
	  {
		method = static_cast<Methods>(i);
		return;
	  }
  method = UNKNOWN_METHOD;
  statusCode = NOT_IMPLEMENTED;
}

// Setup target value + update statusCode if required
void RequestLine::extractRequestTarget (std::string &line)
{
  int index (line.find (" "));

  if (index == 0 || line[0] != '/')
	{
	  statusCode = BAD_REQUEST;
	  return;
	}
  target = std::string (line, 0, index);
  while (httpVersion == "HTTP/1.0" && line[index] == ' ')
	index++; // To remove all spaces if HTTP/1.0
  if (httpVersion == "HTTP/1.1")
	index++;
  line.erase (0, index);
  for (int i (0); target[i]; i++)
	{
	  if (target[i] > 126 || target[i] <= ' ')
		{
		  statusCode = BAD_REQUEST;
		  return;
		}
	}
  if (!isPathSafe (this->target))
	this->statusCode = 400;
}

