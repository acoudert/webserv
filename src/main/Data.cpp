#include "Data.hpp"

Data::Data (char **args) : servers (NULL)
{
  if (args[1]
	  && (std::string (args[1]) == "-h" || std::string (args[1]) == "--help"))
	{
	  displayHelp ();
	  exit (0);
	}
  for (int i (1); args[i]; i++)
	{
	  if (args[i] && (std::string (args[i]) == "-v"
					  || std::string (args[i]) == "--verbose"))
		g_verbose = true;
	  else if (args[i] && (std::string (args[i]) == "-vv"
						   || std::string (args[i]) == "--vverbose"))
		{
		  g_verbose = true;
		  g_vverbose = true;
		}
	  else if (args[i] && (std::string (args[i]) == "-s"
						   || std::string (args[i]) == "--slowloris"))
		g_slowloris = true;
	  else if (args[i] && (std::string (args[i]) == "-t"
						   || std::string (args[i]) == "--timeout"))
		g_timeout = true;
	  else if (args[i] && (std::string (args[i]) == "-p"
						   || std::string (args[i]) == "--pipe"))
		g_pipe = true;
	  else if (i > 6)
		return;
	  else if (args[i] && std::string (args[i]) != "-h"
			   && std::string (args[i]) != "--help")
		servers = ServerGroup (args[i]);
	}
}

Data::~Data ()
{}

void Data::displayHelp () const
{
  std::cout << "Usage: ./webserv [OPTION]... [FILE]" << std::endl;
  std::cout << "Launch an HTTP server based on FILE configuration."
			<< std::endl;
  std::cout << std::endl;

  std::cout << "Options:" << std::endl;
  std::cout << "\t-h , --help     : This help" << std::endl;
  std::cout << "\t-v , --verbose  : Produce verbose output" << std::endl;
  std::cout << "\t-vv, --vverbose : Produce more verbose output" << std::endl;
  std::cout << "\t-s , --slowloris: Activate slowloris protection" << std::endl;
  std::cout << "\t-t , --timeout  : Activate timeout protection" << std::endl;
  std::cout << "\t-p , --pipe     : Increase CGI pipe size" << std::endl;
}
