#include "ResponseHeader.hpp"
#include <cstring>
#include <cassert>
#include <sys/time.h>
#include <ctime>

StatusCodes const        ResponseHeader::codeHelper[STATUSCODE_NB] =
	{
		{OK,                "OK"},
		{BAD_REQUEST,       "BAD_REQUEST"},
		{NOT_FOUND,         "NOT FOUND"},
		{PAYLOAD_TOO_LARGE, "PAYLOAD TOO LARGE"},
		{URI_TOO_LONG,      "URI TOO LONG"},
		{NOT_IMPLEMENTED,   "NOT IMPLEMENTED"},
	};

ResponseHeader::ResponseHeader (short statusCode)
	: version (HTTP_1_1), code (statusCode)
{
  this->set ("Server", "Webserv");
  this->set ("Transfer-Encoding", "identity");
  struct timeval tv;
  struct timezone tz;

  tz.tz_minuteswest = 0;
  tz.tz_dsttime = 0;
  tz.tz_dsttime = 0;
  gettimeofday (&tv, &tz);
  char buf[42];
  tvToStr (buf, &tv);
  this->set ("Date", buf);

  for (short i (0); i < STATUSCODE_NB; i++)
	{
	  if (codeHelper[i].code == code)
		{
		  message = codeHelper[i].codeStr;
		  return;
		}
	}
}

ResponseHeader::ResponseHeader (const ResponseHeader &x)
{
  *this = x;
}

ResponseHeader &ResponseHeader::operator= (const ResponseHeader &x)
{
  this->version = x.version;
  this->message = x.message;
  this->code = x.code;
  this->fields = x.fields;
  return *this;
}

// assumes code >= 100 && code < 1000
// Return response header size 
size_t ResponseHeader::measure () const
{
  size_t s = 4 + 8 + this->message.length () + 5;
  for (std::map<std::string, std::string>::const_iterator it = this->fields.begin ();
	   it != this->fields.end (); ++it)
	{
	  s += 4 + it->first.length () + it->second.length ();
	}
  return s;
}

#include <iostream>
#include <libft.hpp>
// Return buf containing enitre header response
char *ResponseHeader::generate (size_t s)
{
  char *buf = new char[s];
  ft::memcpy (buf, (this->version == HTTP_1_0 ? "HTTP/1.0 " : "HTTP/1.1 "), 9);
  ft::itoa (&buf[9], this->code);
  buf[12] = ' ';
  size_t i = 13;
  ft::memcpy (&buf[i], &this->message[0], this->message.length ());
  i += this->message.length ();
  buf[i++] = '\r';
  buf[i++] = '\n';
  for (std::map<std::string, std::string>::const_iterator it = this->fields.begin ();
	   it != this->fields.end (); ++it)
	{
	  ft::memcpy (&buf[i], &it->first[0], it->first.length ());
	  i += it->first.length ();
	  ft::memcpy (&buf[i], ": ", 2);
	  i += 2;
	  ft::memcpy (&buf[i], &it->second[0], it->second.length ());
	  i += it->second.length ();
	  buf[i++] = '\r';
	  buf[i++] = '\n';
	}
  buf[i++] = '\r';
  buf[i] = '\n';
  return buf;
}

void ResponseHeader::setCode (short code)
{
  assert (code >= 100 && code < 1000);
  this->code = code;
}

void ResponseHeader::setMessage (const std::string &message)
{
  this->message = message;
}

void ResponseHeader::set (const std::string &key, const std::string &value)
{
  this->fields[key] = value;
}

void ResponseHeader::setVersion (Versions v)
{
  this->version = v;
}
unsigned short ResponseHeader::getCode () const
{
  return this->code;
}

