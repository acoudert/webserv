#include "Response.hpp"

Response::Response (Request const &_req)
	: req (_req), _IGenSet (false), _headerSent (false), _fullyWritten (false), _fullySent (false), writeBuf (REQ_BUF_SIZE), IGen (NULL)
{
}

Response::Response (Response const &src)
	: req (src.req), _IGenSet (src._IGenSet), _headerSent (src._headerSent), _fullyWritten (src._fullyWritten), _fullySent (src._fullySent), writeBuf (src.writeBuf), IGen (NULL)
{
  if (src.IGen)
	IGen = src.IGen->clone ();
}

Response &Response::operator= (Response const &src)
{
  req = src.req;
  _IGenSet = src._IGenSet;
  _headerSent = src._headerSent;
  _fullyWritten = src._fullyWritten;
  _fullySent = src._fullySent;
  writeBuf = src.writeBuf;
  return *this;
}

Response::~Response ()
{
  if (IGen)
	delete IGen;
}

bool Response::fullySent () const
{
  return _fullySent;
}

bool
Response::handleResponse (int const &fd, fd_set *selectedFDs, int *toAddFds, int *toCloseFds)
{
  if (!_IGenSet)
	return setIGen (toAddFds);
  onWritable (fd, selectedFDs);
  if (!req.alive ())
	return true;
  return intoWriteBuf (selectedFDs, toCloseFds) && writeBuf.used () == 0;
}

void Response::onWritable (int const &fd, fd_set const *selectedFDs)
{
  if (FD_ISSET(fd, &selectedFDs[1]) && this->writeBuf.used () > 0)
	{
	  ssize_t n = send (fd, this->writeBuf.get (), this->writeBuf.used (), 0);
	  if (n < 0)
		req.setAlive (false);
	  else
		{
		  this->writeBuf.pop (n);
		  req.setLast ();
		}
	}
  if (_fullyWritten && writeBuf.used () == 0)
	{
	  _fullySent = true;
	}
}

bool Response::intoWriteBuf (fd_set *selectedFDs, int *toCloseFds)
{
  char *buf (NULL);
  size_t size (0);

  if (!_headerSent)
	{
	  buf = IGen->getHeader (&size, selectedFDs, toCloseFds);
	  if (buf == NULL)
		return false;
	  writeBuf.reserve (size);
	  writeBody (buf, size);
	  _headerSent = true;
	  delete[] buf;
	}
  if (!_headerSent)
	{
	  if (g_vverbose)
		displayResponse ();
	  return false;
	}
  if (IGen->execute (selectedFDs, toCloseFds))
	{
	  if (g_vverbose)
		displayResponse ();
	  _fullyWritten = true;
	  return true;
	}
  if (g_vverbose)
	displayResponse ();
  return false;
}

size_t Response::writeAvailable () const
{
  return writeBuf.available ();
}

size_t Response::writeBody (char *buf, size_t bufSize)
{
  assert (bufSize <= writeAvailable ());
  writeBuf.push (buf, bufSize);
  return bufSize;
}

bool Response::shouldWriteSelect () const
{
  return writeBuf.used () != 0 || req.fullyParsed ()
		 || req.getReadBuf ().used () != 0;
}

bool Response::cleanInternalData ()
{
  if (!req.alive ())
	return true;
  this->destroy ();
  if (IGen)
	delete IGen;
  IGen = NULL;
  _IGenSet = false;
  _headerSent = false;
  _fullyWritten = false;
  _fullySent = false;
  return req.cleanInternalData ();
}

void Response::displayResponse () const
{
  req.displayRequest ();
  std::cout << "RESPONSE" << std::endl;
  std::cout << "IGenSet: " << std::boolalpha << _IGenSet << std::noboolalpha
			<< std::endl;
  std::cout << "Header Sent: " << std::boolalpha << _headerSent
			<< std::noboolalpha << std::endl;
  std::cout << "Fully Written: " << std::boolalpha << _fullyWritten
			<< std::noboolalpha << std::endl;
  std::cout << "Fully Sent: " << std::boolalpha << _fullySent
			<< std::noboolalpha << std::endl;
  std::cout << "req.Alive: " << std::boolalpha << req.alive ()
			<< std::noboolalpha << std::endl;
  std::cout << "IGen Type: ";
  if (IGen == NULL)
	std::cout << "NULL" << std::endl;
  else if (dynamic_cast<HEADResponse *>(IGen) != NULL)
	std::cout << "HEADResponse" << std::endl;
  else if (dynamic_cast<GETResponse *>(IGen) != NULL)
	std::cout << "GETResponse" << std::endl;
  else if (dynamic_cast<ErrorResponse *>(IGen) != NULL)
	std::cout << "ErrorResponse" << std::endl;
  std::cout << std::endl;
  if (_fullySent)
	std::cout << "----------------------------" << std::endl;
}

Request &Response::getRequest ()
{
  return req;
}

bool Response::setIGen (int *toAddFds)
{
  unsigned short err = req.getStatusCode ();
  Versions v = req.getVersion ();
  std::string allow;

  if (err == 200 && req.get ("Host") == "")
	{
	  if (v == HTTP_1_0)
		err = 999;
	  else
		err = 400;
	}
  if (v == UNKNOWN_VERSION && err == 505)
	{
	  IGen = new ErrorResponse (*this, 505);
	  IGen->setUp (toAddFds);
	}
  else if (err == 200)
	{
	  std::map<Methods, bool> mp;
	  const Location *l = req.getServer ()->getLocation (req, &mp);
	  if (l == NULL && mp[UNKNOWN_METHOD])
		{
		  for (std::map<Methods, bool>::iterator it = mp.begin ();
			   it != mp.end (); ++it)
			{
			  if (it->first != UNKNOWN_METHOD)
				{
				  allow += strMethods[it->first] + ", ";
				}
			}
		  err = 405;
		  goto error;
		}
	  else
		setStandardIGen (toAddFds);
	}
  else
	{
	error:
	if (err == 999)
	  {
		err = 200;
	  }
	  IGen = new ErrorResponse (*this, err);
	  IGen->setUp (toAddFds);
	  if (!allow.empty ())
		{
		  allow.resize (allow.size () - 2);
		  IGen->headerSet ("Allow", allow);
		}
	}
  _IGenSet = true;
  return false;
}

// PUT SAME ORDER AS DEFINED IN METHODS
ftConstructor const respConstructors[METHODS_NB - 1] = {
	&Response::getConstructor,
	&Response::headConstructor,
	&Response::getConstructor, // POST
	&Response::putConstructor
};

void Response::setStandardIGen (int *toAddFds)
{
  for (int i (0); i < METHODS_NB - 1; i++)
	{
	  if (i == static_cast<int>(req.getRLine ().method))
		{
		  IGen = (this->*(respConstructors[i])) ();
		  IGen->setUp (toAddFds);
		  return;
		}
	}
  // Error
}

IResponse *Response::getConstructor ()
{
  const Location *loc = req.getServer ()->getLocation (req, NULL);
  if (loc != NULL)
	return new GETResponse (*req.getServer (), req, req.getRLine (), *loc, *this);
  return new ErrorResponse (*this, 200);
}

IResponse *Response::headConstructor ()
{
  const Location *loc = req.getServer ()->getLocation (req, NULL);
  if (loc != NULL)
	return new HEADResponse (*req.getServer (), req, req.getRLine (), *loc, *this);
  return new ErrorResponse (*this, 200);
}

IResponse *Response::putConstructor ()
{
  const Location *loc = req.getServer ()->getLocation (req, NULL);
  if (loc != NULL)
	return new PUTResponse (*this, *loc);
  return new ErrorResponse (*this, 200);
}

void Response::destroy ()
{
  if (this->IGen != NULL)
	this->IGen->destroy ();
}
