#include <fcntl.h>
#include <libft.hpp>
#include <map>
#include <pathUtils.hpp>
#include "Server.hpp"
#include "FDGenerator.hpp"

ServerSubBlock const serverSubBlock[SERVERSUBBLOCK_NB] = {
	{"listen",      &Server::listenSubBlock},
	{"root",        &Server::rootSubBlock},
	{"index",       &Server::indexSubBlock},
	{"server_name", &Server::serverNameSubBlock},
	{"error_page",  &Server::errorPageSubBlock},
	{"location",    &Server::locationSubBlock}
};

Server::Server () : listenPort (0), root (""), error (false)
{}

Server::Server (std::ifstream &file, std::string &line)
	: listenPort (0), root (""), error (false)
{
  if (!extractServer (file, line))
	error = true;
}

Server::Server (const Server &x)
	: serverNames ()
{
  *this = x;
}

Server::~Server ()
{}

// To update once Server class is finished
Server &Server::operator= (const Server &x)
{
  this->listenPort = x.listenPort;
  this->root = x.root;
  this->serverNames = x.serverNames;
  this->indexes = x.indexes;
  this->errorPages = x.errorPages;
  this->locations = x.locations;
  return *this;
}

void Server::displayServer () const
{
  std::cout << "\t\terror: " << std::boolalpha << error << std::noboolalpha
			<< std::endl;
  std::cout << "\t\tlistenPort: " << listenPort << std::endl;
  std::cout << "\t\troot: " << root << std::endl;
  std::cout << "\t\tServer Names:" << std::endl;
  for (std::vector<std::string>::const_iterator it (serverNames.begin ());
	   it != serverNames.end (); ++it)
	std::cout << "\t\t\t" << *it << std::endl;
  std::cout << "\t\tIndexes:" << std::endl;
  for (std::vector<std::string>::const_iterator it (indexes.begin ());
	   it != indexes.end (); ++it)
	std::cout << "\t\t\t" << *it << std::endl;
  std::cout << "\t\tError Pages:" << std::endl;
  for (std::map<unsigned short, std::string>::const_iterator it (errorPages.begin ());
	   it != errorPages.end (); ++it)
	std::cout << "\t\t\t[" << it->first << "] " << it->second << std::endl;
  for (size_t i (0); i < locations.size (); i++)
	{
	  std::cout << "\t\tlocation " << i << ":" << std::endl;
	  locations[i].displayLocation ();
	}
}

bool Server::errorConf () const
{
  return error;
}

in_port_t const &Server::getListenPort () const
{
  return listenPort;
}

std::string const &Server::getRoot () const
{
  return root;
}

std::string const &Server::getServerName (size_t const &i) const
{
  return serverNames[i];
}

size_t Server::getServerNamesSize () const
{
  return serverNames.size ();
}

std::string const &Server::getIndex (size_t const &i) const
{
  return indexes[i];
}

size_t Server::getIndexesSize () const
{
  return indexes.size ();
}

// Return errorPage.second if found, else indexes[0] 
std::string const &Server::getErrorPage (unsigned short const &errorCode) const
{
  return errorPages.find (errorCode) != errorPages.end () ?
		 errorPages.find (errorCode)->second : indexes[0];
}

std::map<unsigned short, std::string> const &Server::getErrorPagesMap () const
{
  return errorPages;
}

size_t Server::getErrorPagesSize () const
{
  return errorPages.size ();
}

Location const &Server::getLocation (size_t const &i) const
{
  return locations[i];
}

size_t Server::getLocationsSize () const
{
  return locations.size ();
}

void Server::addRootLocation ()
{
  for (size_t i (0); i < locations.size (); i++)
	{
	  if (locations[i].getLocation () == "/")
		return;
	}
  locations.push_back (Location ("/", root));
}

// Extract entire server, update line and file accordingly
bool Server::extractServer (std::ifstream &file, std::string &line)
{
  std::string subBlockStr;
  size_t endIndex;

  line.erase (0, 6); // erase "server"
  if (line[0] != ' ' && line[0] != '\t')
	return false;
  line = strip (line, " \t"); // remove ' ' && '\t'
  if (line[0] != '{' || line.length () != 1)
	return false; // error if "{" not final char or if contains any other char
  while (!file.eof ())
	{
	  std::getline (file, line);
	  skipEmptyLines (file, line);
	  if (line == "}")
		break;
	  endIndex = findIndexChar (line, " \t");
	  subBlockStr = std::string (line, 0, endIndex);
	  for (int i (0); i < SERVERSUBBLOCK_NB; i++)
		{
		  if (subBlockStr == serverSubBlock[i].name)
			{
			  if (!(this->*(serverSubBlock[i].ft)) (file, line))
				return false;
			  break;
			}
		  if (i + 1 == SERVERSUBBLOCK_NB) // error file config if unknown field
			return false;
		}
	}
  if (line != "}")
	return false; // error if eof and not end of server block
  if (listenPort == 0)
	return false; // error if no listen field
  if (root == "")
	return false; // error if no root field
  //addRootLocation ();
  return true;
}

// Extract Listen Info and update file and line accordingly
bool Server::listenSubBlock (std::ifstream &file, std::string &line)
{
  std::string::iterator it;

  (void) file;
  if (listenPort != 0)
	return false; // error if several listen definitions
  line.erase (0, 6); // erase "listen"
  line = strip (line, " \t");
  if (line[line.length () - 1] != ';')
	return false; // error if line doesnt end with ';'
  line = strip (line, " \t;");
  listenPort = static_cast<in_port_t>(ft::atoi (line.c_str ()));
  if (listenPort == 0)
	return false; // error if defined port is 0
  it = line.begin ();
  while (isNumber (*it))
	++it;
  line.erase (line.begin (), it); // erase numbers
  line = strip (line, " \t");
  if (line.length () != 0)
	return false; // error if more than one information in listen field
  return true;
}

// Extract Root Info and update file and line accordingly
bool Server::rootSubBlock (std::ifstream &file, std::string &line)
{
  (void) file;
  if (root != "")
	return false; // error if several listen definitions
  line.erase (0, 4); // erase "root"
  line = strip (line, " \t");
  if (line[line.length () - 1] != ';')
	return false;
  line = strip (line, " \t;");
  if (line.length () == 0)
	return false; // error if no root path precised
  if (findIndexChar (line, " \t") != line.length ())
	return false; // error if more than one information in root field
  if (line[0] != '/' && line[0] != '.')
	return false;
  if (line[0] == '.' && line.length () == 1)
	return false;
  if (line[0] == '.' && line[1] != '.' && line[1] != '/')
	return false;
  if (line[0] == '.' && line[1] == '.' && line.length () == 2)
	return false;
  if (line[0] == '.' && line[1] == '.' && line[2] != '/')
	return false;
  if (line.length () != 1 && line[line.length () - 1] == '/')
	return false; // error if path ends with '/' and not file_system root
  root = line;
  return true;
}

// Extract Index Info and update file and line accordingly
bool Server::indexSubBlock (std::ifstream &file, std::string &line)
{
  size_t endIndex;

  (void) file;
  if (indexes.size () != 0)
	return false; // error if several index definitions, need to check if nginx do the same
  line.erase (0, 5); // erase "index"
  line = strip (line, " \t");
  if (line[line.length () - 1] != ';')
	return false; // error if doesnt end with ';'
  line = strip (line, " \t;");
  while (line.length () != 0)
	{
	  line = strip (line, " \t");
	  endIndex = findIndexChar (line, " \t");
	  indexes.push_back (std::string (line.c_str (), endIndex));
	  line.erase (0, endIndex);
	}
  if (indexes.size () == 0)
	return false; // error if no index
  return true;
}

// Extract Server_name Info and update file and line accordingly
bool Server::serverNameSubBlock (std::ifstream &file, std::string &line)
{
  size_t endIndex;

  (void) file;
  if (serverNames.size () != 0)
	return false; // error if several server_name definitions
  line.erase (0, 11); // erase "server_name"
  line = strip (line, " \t");
  if (line[line.length () - 1] != ';')
	return false; // error if doesnt end with ';'
  line = strip (line, " \t;");
  while (line.length () != 0)
	{
	  line = strip (line, " \t");
	  endIndex = findIndexChar (line, " \t");
	  serverNames.push_back (std::string (line.c_str (), endIndex));
	  line.erase (0, endIndex);
	}
  if (serverNames.size () == 0)
	return false; // error if no index
  return true;
}

// Extract Error_page Info and update file and line accordingly
bool Server::errorPageSubBlock (std::ifstream &file, std::string &line)
{
  std::string page;
  unsigned short errorCode;
  size_t index (0);

  (void) file;
  if (errorPages.size () != 0)
	return false; // error if several errorPages definitions
  line.erase (0, 10); // erase "error_page"
  line = strip (line, " \t");
  if (line[line.length () - 1] != ';')
	return false;
  line = strip (line, " \t;");
  if (line.find (" ") == std::string::npos
	  && line.find ("\t") == std::string::npos)
	return false; // error if only one info field
  if (line.rfind (" ") != std::string::npos)
	index = line.rfind (" ");
  if (line.rfind ("\t") != std::string::npos && line.rfind ("\t") > index)
	index = line.rfind ("\t");
  page = std::string (line, index + 1);
  line.erase (index);
  line = strip (line, " \t");
  while (line.length () != 0)
	{
	  errorCode = static_cast<unsigned short>(ft::atoi (line.c_str ()));
	  if (errorCode < 400
		  || errorCode >= 600) // May require to improve value check
		return false;
	  errorPages.insert (std::pair<unsigned short, std::string> (errorCode, page));
	  index = 0;
	  while (line[index] >= '0' && line[index] <= '9')
		index++;
	  line.erase (0, index);
	  line = strip (line, " \t");
	}
  return true;
}

// Extract location Info and update file and line accordingly
bool Server::locationSubBlock (std::ifstream &file, std::string &line)
{
  Location location (file, line);

  if (location.errorConf ())
	return false;
  locations.push_back (location);
  return true;
}

bool Server::matches (const Request &r) const
{
  assert (this->canBeDefault (r) == TRUE);
  if (r.get ("Host").empty ())
	return false;
  for (std::vector<Location>::const_iterator it = this->locations.begin ();
	   it != this->locations.end (); ++it)
	{
	  if (it->matches (r, NULL) != 0)
		return true;
	}
  return false;
}

int Server::canBeDefault (const Request &r) const
{
  std::string host = r.get ("Host");
  size_t portBegin;
  t_port port;
  for (std::vector<std::string>::const_iterator it = this->serverNames.begin ();
	   it != this->serverNames.end (); ++it)
	{
	  for (size_t i = 0;; ++i)
		{
		  if ((*it)[i] == '\0' && host[i] == ':')
			{
			  portBegin = i;
			  goto ok;
			}
		  if ((*it)[i] != host[i])
			{
			  break;
			}
		  if ((*it)[i] == '\0' && host[i] == '\0')
			{
			  port = 80;
			  goto port;
			}
		}
	}
  port = r.getPort ();
  goto port;
  ok:
  port = t_port (ft::atoi (&host[portBegin + 1]));
  port:
  if (port != r.getPort ())
	{
	  std::cout << "Host poisoning detected, dropping request." << std::endl;
	  return POISONING;
	}
  return (port == this->listenPort ? TRUE : FALSE);
}

std::string Server::getFileName (const Request &req) const
{
  std::map<Methods, bool> mp;
  assert(this->getLocation (req, &mp) != NULL);
  return this->getLocation (req, &mp)->getFileName (req);
}

IGenerator *Server::errorGenerator (unsigned short code) const
{
  std::map<unsigned short, std::string>::const_iterator it = this->errorPages.find (code);
/*  if (it != this->errorPages.end ())
	{
	  FDGenerator *g = new FDGenerator (it->second);
	  if (g->getFd () != -1)
		return g;
	  delete g;
	}
*/
  return new ErrorGenerator (code);
}

Location const *
Server::getLocation (const Request &r, std::map<Methods, bool> *meths) const
{
  assert (this->canBeDefault (r));
  const Location *bestL;
  int best = CANARY;
  for (std::vector<Location>::const_iterator it = this->locations.begin ();
	   it != this->locations.end (); ++it)
	{
	  std::map<Methods, bool> tmp;
	  int m = it->matches (r, &tmp);
	  if (m == CANARY)
		continue;
	  if ((ft::abs (m) == ft::abs (best) && m <= best) && meths != NULL
		  && best != CANARY)
		{
		  for (std::map<Methods, bool>::iterator it = tmp.begin ();
			   it != tmp.end (); ++it)
			(*meths)[it->first] = it->second;
		}
	  else if (ft::abs (m) > ft::abs (best)
			   || (ft::abs (m) == ft::abs (best) && m > best)
			   || best == CANARY)
		{
		  if (meths != NULL)
			*meths = tmp;
		  bestL = &(*it);
		  best = m;
		}
	}
  if (best > 0)
	return bestL;
  if (meths != NULL)
	(*meths)[UNKNOWN_METHOD] = true;
  return NULL;
}

IGenerator *
Server::getError (unsigned short code, int *toAddFds, int *newFd) const
{
  std::map<unsigned short, std::string>::const_iterator it = this->errorPages.find (code);
  if (it != this->errorPages.end ())
	{

	  std::string str = it->second;
	  str = pathAppend (this->root, str);
	  int fd = open (str.c_str (), O_RDONLY);
	  if (fd > 0)
		{
		  toAddFds[0] = fd;
		  *newFd = fd;
		  return new FDGenerator (fd, str);
		}
	  IGenerator *gen = new ErrorGenerator (500);
	  gen->assignStatusCode (500);
	  return gen;
	}
  return new ErrorGenerator (code);
}
