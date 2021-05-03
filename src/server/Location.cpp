#include <pathUtils.hpp>
#include <libft.hpp>
#include "Location.hpp"

LocationSubBlock const locationSubBlock[LOCATIONSUBBLOCK_NB] = {
	{"root",                 &Location::rootSubBlock},
	{"index",                &Location::indexSubBlock},
	{"methods",              &Location::methodsSubBlock},
	{"autoindex",            &Location::autoIndexSubBlock},
	{"cgi",                  &Location::cgiSubBlock},
	{"save_dir",             &Location::saveDirSubBlock},
	{"client_max_body_size", &Location::clientMaxBodySizeSubBlock}
};

Location::Location ()
	: location (""), root (""), autoIndex (OFF), saveDir (""), maxBodySize (0), error (false)
{}

Location::Location (std::string const &location, std::string const &root)
	: location (location), root (root), autoIndex (OFF), saveDir (""), maxBodySize (0), error (false)
{
  acceptedMethods.push_back (static_cast<Methods>(0));
  acceptedMethods.push_back (static_cast<Methods>(1));
}

Location::Location (std::ifstream &file, std::string &line)
	: location (""), root (""), autoIndex (OFF), saveDir (""), maxBodySize (0), error (false)
{
  if (!extractLocation (file, line))
	error = true;
}

Location::Location (Location const &x)
{
  *this = x;
}

Location::~Location ()
{}

Location &Location::operator= (Location const &x)
{
  location = x.location;
  root = x.root;
  indexes = x.indexes;
  acceptedMethods = x.acceptedMethods;
  autoIndex = x.autoIndex;
  CGIs = x.CGIs;
  saveDir = x.saveDir;
  maxBodySize = x.maxBodySize;
  error = x.error;
  return *this;
}

// Extract entire location, update line and file accordingly
bool Location::extractLocation (std::ifstream &file, std::string &line)
{
  std::string subBlockStr;
  size_t endIndex;

  line.erase (0, 8); // erase "location"
  if (line[0] != ' ' && line[0] != '\t')
	return false;
  line = strip (line, " \t");
  if (line[line.length () - 1] != '{')
	return false;
  if (line.length () > 1 && line[line.length () - 2] != ' '
	  && line[line.length () - 2] != '\t')
	return false; // error if no ' ' or '\t' before '{'
  line = strip (line, " \t{");
  if (line.length () == 0 || line[0] != '/')
	return false; // error if no location path
  location = line;
  while (!file.eof ())
	{
	  std::getline (file, line);
	  skipEmptyLines (file, line);
	  if (line == "}")
		break;
	  endIndex = findIndexChar (line, " \t");
	  subBlockStr = std::string (line, 0, endIndex);
	  for (int i (0); i < LOCATIONSUBBLOCK_NB; i++)
		{
		  if (subBlockStr == locationSubBlock[i].name)
			{
			  if (!(this->*(locationSubBlock[i].ft)) (file, line))
				return false;
			  break;
			}
		  if (i + 1
			  == LOCATIONSUBBLOCK_NB) // error file config if unknown field
			return false;
		}
	}
  if (line != "}")
	return false; // error if doesnt end with '}' (unecessary check as Server would get an error)
  if (acceptedMethods.size () == 0)
	{
	  acceptedMethods.push_back (static_cast<Methods>(0));
	  acceptedMethods.push_back (static_cast<Methods>(1));
	}
  while (this->location.length () != 0 &&
		 this->location[this->location.length () - 1] == '/')
	this->location.erase (this->location.end () - 1);
  for (size_t i (0); i < acceptedMethods.size (); i++)
	{
	  if (acceptedMethods[i] == PUT)
		{
		  if (saveDir == "")
			return false;
		}
	}
  if (maxBodySize == 0)
	maxBodySize = DEFAULT_MAXBODYSIZE;
  return true;
}

void Location::displayLocation () const
{
  std::cout << "\t\t\terror: " << std::boolalpha << error << std::noboolalpha
			<< std::endl;
  std::cout << "\t\t\tlocation: " << location << std::endl;
  std::cout << "\t\t\troot: " << root << std::endl;
  std::cout << "\t\t\tIndexes: " << std::endl;
  for (std::vector<std::string>::const_iterator it (indexes.begin ());
	   it != indexes.end (); ++it)
	std::cout << "\t\t\t\t" << *it << std::endl;
  std::cout << "\t\t\tautoIndex: ";
  if (autoIndex == ON)
	std::cout << "ON" << std::endl;
  else
	std::cout << "OFF" << std::endl;
  std::cout << "\t\t\tmaxBodySize: " << maxBodySize << std::endl;
  std::cout << "\t\t\tacceptedMethods.size(): " << acceptedMethods.size ()
			<< std::endl;
  std::cout << "\t\t\tacceptedMethods: " << std::endl;
  for (size_t i (0); i < acceptedMethods.size (); i++)
	std::cout << "\t\t\t\t[" << i << "] " << strMethods[acceptedMethods[i]]
			  << std::endl;
  std::cout << "\t\t\tsave_dir: " << saveDir << std::endl;
  for (size_t i (0); i < CGIs.size (); i++)
	{
	  std::cout << "\t\t\tCGI " << i << ":" << std::endl;
	  CGIs[i].displayCGI ();
	}
}

bool Location::errorConf () const
{
  return error;
}

std::string const &Location::getLocation () const
{
  return location;
}

std::string const &Location::getRoot () const
{
  return root;
}

Methods const &Location::getAcceptedMethod (size_t const &i) const
{
  return acceptedMethods[i];
}

size_t Location::getAcceptedMethodsSize () const
{
  return acceptedMethods.size ();
}

bool const &Location::getAutoIndex () const
{
  return autoIndex;
}

CGI const &Location::getCGI (size_t const &i) const
{
  return CGIs[i];
}

size_t Location::getCGIsSize () const
{
  return CGIs.size ();
}

size_t Location::getMaxBodySize () const
{
  return maxBodySize;
}

bool Location::rootSubBlock (std::ifstream &file, std::string &line)
{
  (void) file;
  if (root != "")
	return false; // error if several root definitions
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
  while (this->root.length () != 0 &&
		 this->root[this->root.length () - 1] == '/')
	this->root.erase (this->location.end () - 1);
  return true;
}

// Extract Index Info and update file and line accordingly
bool Location::indexSubBlock (std::ifstream &file, std::string &line)
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

bool Location::methodsSubBlock (std::ifstream &file, std::string &line)
{
  size_t endIndex;
  std::string method;

  (void) file;
  if (acceptedMethods.size () != 0)
	return false;
  line.erase (0, 7); // erase "methods"
  line = strip (line, " \t");
  if (line[line.length () - 1] != ';')
	return false;
  line = strip (line, " \t;");
  while (line.length () != 0)
	{
	  endIndex = findIndexChar (line, " \t");
	  method = std::string (line, 0, endIndex);
	  for (int i (0); i < METHODS_NB - 1; i++)
		{
		  if (method == strMethods[i])
			{
			  for (size_t j (0); j < acceptedMethods.size (); j++)
				{
				  if (acceptedMethods[j] == i)
					return false;
				}
			  acceptedMethods.push_back (static_cast<Methods>(i));
			  break;
			}
		  if (i == METHODS_NB - 2)
			return false; // false if unknown method
		}
	  line.erase (0, endIndex);
	  line = strip (line, " \t");
	}
  if (acceptedMethods.size () == 0)
	return false; // error if no accepted method
  return true;
}

bool Location::autoIndexSubBlock (std::ifstream &file, std::string &line)
{
  (void) file;
  line.erase (0, 9); // erase "autoindex"
  line = strip (line, " \t");
  if (line[line.length () - 1] != ';')
	return false;
  line = strip (line, " \t;");
  if (line == "on")
	autoIndex = ON;
  else if (line == "off")
	autoIndex = OFF;
  else
	return false;
  return true;
}

bool Location::cgiSubBlock (std::ifstream &file, std::string &line)
{
  CGI cgi (file, line);

  if (cgi.errorConf ())
	return false;
  CGIs.push_back (cgi);
  return true;
}

bool Location::saveDirSubBlock (std::ifstream &file, std::string &line)
{
  (void) file;
  if (saveDir != "")
	return false; // error if several root definitions
  line.erase (0, 8); // erase "save_dir"
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
  saveDir = line;
  while (this->saveDir.length () != 0 &&
		 this->saveDir[this->saveDir.length () - 1] == '/')
	this->saveDir.erase (this->location.end () - 1);
  return true;
}

// Extract Client_max_body_size Info and update file and line accordingly
bool
Location::clientMaxBodySizeSubBlock (std::ifstream &file, std::string &line)
{
  size_t index (0);

  (void) file;
  if (maxBodySize != 0)
	return false; // error if several client_max_body_size definitions
  line.erase (0, 20); // erase "client_max_body_size"
  line = strip (line, " \t");
  if (line[line.length () - 1] != ';')
	return false;
  line = strip (line, " \t;");
  maxBodySize = static_cast<size_t>(ft::atoi (line.c_str ()));
  if (maxBodySize == 0)
	return false;
  while (line[index] >= '0' && line[index] <= '9')
	index++;
  if (index != line.length ())
	return false; // error if anything else than digits
  return true;
}

static bool match (const char *s1, const char *s2)
{
  if (*s1 == '\0' && *s2 == '\0')
	return (true);
  else if (*s1 == *s2 && *s1 != '\0')
	return (match (s1 + 1, s2 + 1));
  else if (*s2 == '*' && *s1 != '\0')
	return (match (s1 + 1, s2) || match (s1 + 1, s2 + 1));
  else
	return (false);
}

static int locationMatch (const char *s1, const char *s2)
{
  const char *cpy = s1;

  while (*s1 != '\0' && *s2 != '\0' && *s1 == *s2)
	{
	  ++s1;
	  ++s2;
	}
  return s1 - cpy;
}

unsigned long long int
Location::matches (const Request &r, std::map<Methods, bool> *meths) const
{
  int n = locationMatch (&(r.getUrl ()[0]), &this->location[0]);

  if ((n == 0 && this->location.length () == 0) || n != 0)
	n++;
  if (n == 0)
	return CANARY;
  for (std::vector<Methods>::const_iterator it = this->acceptedMethods.begin ();
	   it != this->acceptedMethods.end (); ++it)
	{
	  if (meths != NULL)
		(*meths)[*it] = true;
	  if (*it == r.getMethod ())
		return (n == int (this->location.length () + 1) ? n : 0);
	}
  if (n == int (this->location.length () + 1))
	return -n;
  return CANARY;
}

std::string const &Location::getIndex (size_t const &i) const
{
  return indexes[i];
}

size_t Location::getIndexesSize () const
{
  return indexes.size ();
}

std::string Location::getFileName (const Request &r) const
{
  std::string p = r.getUrl ();

  p.erase (0, this->location.length () + 1);
  return pathAppend (this->root, p);
}

std::string Location::getCGIPath (std::string const &filePath) const
{
  for (std::vector<CGI>::const_iterator it = this->CGIs.begin ();
	   it != this->CGIs.end (); ++it)
	{
	  if (match (filePath.c_str (), it->getWildcard ().c_str ()))
		return it->getPathExecutable ();
	}
  return std::string ("");
}

const std::string &Location::getSaveDir () const
{
  return this->saveDir;
}
