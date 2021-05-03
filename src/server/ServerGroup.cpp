#include <libft.hpp>
#include "ServerGroup.hpp"

ServerGroup::ServerGroup (char const *file_name)
	: thr_nb (1), thr_nb_defined (false), error (false)
{
  std::ifstream file;

  if (file_name && std::string (file_name) != "-v")
	file.open (file_name, std::ifstream::in);
  else
	file.open (DEFAULT_CONF, std::ifstream::in);
  if (file.is_open ())
	{
	  while (!file.eof ())
		if (!extractBlock (file))
		  {
			error = true;
			return;
		  }
	}
  else
	error = true;
  if (servers.size () == 0)
	error = true; // error if no servers defined
}

ServerGroup::ServerGroup (ServerGroup const &x)
{
  *this = x;
}

ServerGroup::~ServerGroup ()
{}

ServerGroup &ServerGroup::operator= (ServerGroup const &x)
{
  servers = x.servers;
  thr_nb = x.thr_nb;
  error = x.error;
  return *this;
}

const Server *ServerGroup::getServer (const Request &r) const
{
  Server const *serv = NULL;

  for (std::vector<Server>::const_iterator it = this->servers.begin ();
	   it != this->servers.end (); ++it)
	{
	  int def = it->canBeDefault (r);
	  switch (def)
		{
		  case FALSE:
			break;
		  case TRUE:
			if (serv == NULL)
			  serv = &*it;
		  if (it->matches (r))
			return &*it;
		  break;
		  default:
			break;
		}
	}
  return serv;
}

Server &ServerGroup::operator[] (size_t const &i)
{
  return servers[i];
}

size_t ServerGroup::getSize () const
{
  return servers.size ();
}

size_t ServerGroup::getThrNb () const
{
  return thr_nb;
}

bool ServerGroup::errorConf () const
{
  return error;
}

void ServerGroup::displayServerGroup () const
{
  std::cout << "SERVER GROUP" << std::endl;
  std::cout << "\terror: " << std::boolalpha << errorConf () << std::noboolalpha
			<< std::endl;
  std::cout << "\tthr_nb: " << getThrNb () << std::endl;
  for (size_t i (0); i < getSize (); i++)
	{
	  std::cout << "\tServer " << i << ":" << std::endl;
	  servers[i].displayServer ();
	}
}

bool ServerGroup::extractBlock (std::ifstream &file)
{
  std::string line;

  std::getline (file, line);
  skipEmptyLines (file, line);
  if (line.find ("workers") == 0)
	{
	  if (!extractThrNb (line))
		return false;
	}
  else if (line.find ("server") == 0)
	{
	  Server server (file, line);
	  if (server.errorConf ())
		return false;
	  servers.push_back (server);
	}
  else if (!file.eof ())
	return false;
  return true;
}

bool ServerGroup::extractThrNb (std::string &line)
{
  std::string::iterator it;

  if (thr_nb_defined) // Server error if 2 workers line
	return false;
  thr_nb_defined = true;
  line.erase (0, 7); // erase "workers"
  if (line[0] != ' ' && line[0] != '\t')
	return false;
  line = strip (line, " \t"); // remove ' ' && '\t'
  if (line[line.length () - 1] != ';')
	return false; // last char not ; => error
  line = strip (line, " \t;"); // remove ' ' && '\t' && ';'
  if (!(thr_nb = static_cast<size_t>(ft::atoi (line.c_str ())))) // get Thr_nb, return false if 0
	return false;
  it = line.begin ();
  while (isNumber (*it))
	++it;
  line.erase (line.begin (), it); // erase numbers
  line = strip (line, " \t");
  if (line.length () != 0)
	return false; // error if contains anything else than numbers (including '-' and '+')
  return true;
}
