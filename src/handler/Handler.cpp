#include "Handler.hpp"

Handler::Handler ()
{}

Handler::~Handler ()
{}

void Handler::AddRequest (Request &r, int fd, t_port const &port)
{
  this->listCon.push_back (Connection (fd, r, port));
}

bool
Handler::signal (int const &fd, fd_set *selectedFDs, int *toAddFds, int *toCloseFds)
{
  std::list<Connection>::iterator it (findConnectionIt (fd));

  return this->handleConnection (*it, selectedFDs, toAddFds, toCloseFds);
}

bool
Handler::handleConnection (Connection &c, fd_set *selectedFDs, int *toAddFds, int *toCloseFds)
{
  if (!c.resp.getRequest ().fullyParsed ())
	c.resp.getRequest ().onReadable (c.fd, selectedFDs);
  if (!c.resp.getRequest ().alive ())
	return true;
  if (c.resp.getRequest ().fullyParsed () && !c.resp.fullySent ())
	return c.resp.handleResponse (c.fd, selectedFDs, toAddFds, toCloseFds);
  return false;
}

bool Handler::shouldWriteSelect (int fd)
{
  std::list<Connection>::iterator it (findConnectionIt (fd));

  return it->resp.shouldWriteSelect ();
}

void Handler::eraseRequest (int const &fd)
{
  std::list<Connection>::iterator it (findConnectionIt (fd));

  listCon.erase (it);
}

bool Handler::cleanData (int const &fd)
{
  std::list<Connection>::iterator it (findConnectionIt (fd));

  return it->resp.cleanInternalData ();
}

std::list<Connection>::iterator Handler::findConnectionIt (int const &fd)
{
  for (std::list<Connection>::iterator it (listCon.begin ());
	   it != listCon.end (); ++it)
	{
	  if (it->fd == fd)
		return it;
	}
  return listCon.begin ();
}
