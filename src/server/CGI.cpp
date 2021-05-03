#include "CGI.hpp"

CGI::CGI () : wildcard (""), pathExecutable (""), error (false)
{}

CGI::CGI (std::ifstream &file, std::string &line)
	: wildcard (""), pathExecutable (""), error (false)
{
  if (!extractCGI (file, line))
	error = true;
}

CGI::CGI (const CGI &x)
{
  *this = x;
}

CGI::~CGI ()
{}

CGI &CGI::operator= (CGI const &x)
{
  wildcard = x.wildcard;
  pathExecutable = x.pathExecutable;
  error = x.error;
  return *this;
}

void CGI::displayCGI () const
{
  std::cout << "\t\t\t\terror: " << std::boolalpha << error << std::noboolalpha
			<< std::endl;
  std::cout << "\t\t\t\twildcard: " << wildcard << std::endl;
  std::cout << "\t\t\t\tpathExecutable: " << pathExecutable << std::endl;
}

bool CGI::errorConf () const
{
  return error;
}

std::string const &CGI::getWildcard () const
{
  return wildcard;
}

std::string const &CGI::getPathExecutable () const
{
  return pathExecutable;
}

bool CGI::extractCGI (std::ifstream &file, std::string &line)
{
  size_t endIndex;

  (void) file;
  line.erase (0, 3); // erase"cgi"
  line = strip (line, " \t");
  if (line[line.length () - 1] != ';')
	return false;
  line = strip (line, " \t;");
  endIndex = findIndexChar (line, " \t");
  if (endIndex == line.length ())
	return false;
  wildcard = std::string (line, 0, endIndex);
  if (wildcard.find ("*") == std::string::npos)
	return false;
  line = std::string (line, endIndex);
  line = strip (line, " \t");
  if (line.find (" ") != std::string::npos
	  || line.find ("\t") != std::string::npos)
	return false;
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
  if (line.length () < 2)
	return false;
  if (line[line.length () - 1] == '/')
	return false;
  pathExecutable = line;
  return true;
}
