#include "CGIGenerator.hpp"

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>
#include <errno.h>
#include <cstring>
#include <unistd.h>
#include <libft.hpp>

// server storage may be unecessary
// assumption cgiPath exists
// assumption filePath readable
CGIGenerator::CGIGenerator (Server const &s, Request &r, RequestLine const &rL, const Location &loc, int _fd, int *inpipe, int *outpipe, std::string const &_filePath, std::string const &cgi, ResponseHeader &_resp)
	: server (s), request (r), requestLine (rL), location (loc), argv (NULL), envp (NULL), pid (-1), fd (_fd), inPipe (inpipe), outPipe (outpipe), file (""), filePath (_filePath), forkLaunched (false), forkStdin (false), cgiPath (cgi), resp (_resp), handled (false), envStr (NULL)
{
  setUpArgv ();
  setUpEnvp ();
}

CGIGenerator::CGIGenerator (const CGIGenerator &x)
	: server (x.server), request (x.request), requestLine (x.requestLine), location (x.location), argv (NULL), envp (NULL), pid (x.pid), inPipe (x.inPipe), outPipe (x.outPipe), resp (x.resp), envStr (NULL)
{
  *this = x;
}

CGIGenerator &CGIGenerator::operator= (const CGIGenerator &x)
{
  this->fd = x.fd;
  this->file = x.file;
  this->filePath = x.filePath;
  this->forkLaunched = x.forkLaunched;
  this->forkStdin = x.forkStdin;
  this->cgiPath = x.cgiPath;
  this->handled = x.handled;
  this->pid = x.pid;
  setUpArgv (); // Avoid Segfault + Leaks
  setUpEnvp (); // Same
  return *this;
}

CGIGenerator::~CGIGenerator ()
{
  if (argv)
	{
	  for (short i (0); i < 3; i++)
		delete[] argv[i];
	  delete[] argv;
	}
  if (envp)
	{
	  for (size_t i (0); i < FIELD_NB + request.getHeaders ().size (); i++)
		delete[] envp[i];
	  delete[] envp;
	}
  if (envStr)
	delete[] envStr;
}

// Return buf based on what it could read from fd stored in CGIGenerator
// Close file_fd if everything has been read
ssize_t CGIGenerator::Read (char *buf, size_t s, fd_set const *selectedFDs)
{
  ssize_t i (0);

  (void) selectedFDs;
  while (file[i])
	{
	  if (size_t (i) == s)
		{
		  file.erase (0, i);
		  return i;
		}
	  buf[i] = file[i];
	  i++;
	}
  file.erase (0, i);
  handled = true;
  return i;
}

// Retrieve entire file content from fork
ssize_t CGIGenerator::getSize (fd_set const *selectedFDs, int *toCloseFds)
{
  char buf[8192] = {0};
  ssize_t s;

  if (!forkLaunched)
	forkLauncher (selectedFDs);
  if (!forkStdin)
	forkWrite (selectedFDs, toCloseFds);
  // NEXT COMMENTED OUT TO ALLOW THREADING TEST
  if (this->pid > 0)
	{
	  if (waitpid (this->pid, NULL, WNOHANG) == this->pid)
		{
		  toCloseFds[4] = this->outPipe[1];
		  close (this->outPipe[1]);
		  this->outPipe[1] = -1;
		  this->pid = -1;
		}
	}

  if (FD_ISSET(outPipe[0], &selectedFDs[0]))
	{
	  s = read (outPipe[0], buf, 8191);
	  if (s < 0)
		throw std::exception ();
	}
  else
	return -1;
  file += buf;
  if (s == 0)
	{
	  handleCGIHeaders ();
	  return file.length ();
	}
  return -1;
}

IGenerator *CGIGenerator::clone () const
{
  return new CGIGenerator (*this);
}

void CGIGenerator::forkLauncher (fd_set const *selectedFDs)
{
  if (!FD_ISSET(outPipe[1], &selectedFDs[1])
	  || !FD_ISSET(inPipe[1], &selectedFDs[1]))
	return;
  if (fd != -1)
	{
	  if (!FD_ISSET(fd, &selectedFDs[0]))
		return;
	}
  this->pid = fork ();
  if (this->pid < 0)
	throw std::exception (); // To catch by caller
  forkLaunched = true;
  if (this->pid == 0)
	{
	  // add stdin for post
	  // FIND A WAY TO HANDLE SUCH SYSERROR, WAITPID IS NOT AN OPTION (65000+)
	  if (dup2 (inPipe[0], STDIN_FILENO) < 0)
		exit (1);
	  if (dup2 (outPipe[1], STDOUT_FILENO) < 0)
		exit (1);
	  if (execve (argv[0], argv, envp) < 0)
		{
		  // TONS OF LEAKS IF FAILURE
		  close (fd);
		  close (inPipe[0]);
		  close (inPipe[1]);
		  close (outPipe[0]);
		  close (outPipe[1]);
		  exit (1);
		}
	}
}

void CGIGenerator::forkWrite (fd_set const *selectedFDs, int *toCloseFds)
{
  if (FD_ISSET(inPipe[1], &selectedFDs[1]))
	{
	  if (request.getBody ().begin () == request.getBody ().end ())
		{
		  forkStdin = true;
		  return;
		}
	  char *bodyRef = &(*request.getBody ().begin ());
	  fcntl (inPipe[1], F_SETFL, O_NONBLOCK);
	  ssize_t n = write (inPipe[1], bodyRef, request.getBody ().size ());
	  if (n < 0)
		std::cout << "CGI Failure: " << strerror (errno) << std::endl;
	  std::vector<char>::iterator it = request.getBody ().begin ();
	  if (g_verbose)
		std::cout << "inPipe CGI body.capacity() = "
				  << request.getBody ().capacity () << std::endl;
	  while (n-- > 0)
		++it;
	  request.getBody ().erase (request.getBody ().begin (), it);
	  std::vector<char> (request.getBody ()).swap (request.getBody ());
	  if (request.getBody ().size () == 0)
		{
		  toCloseFds[1] = this->inPipe[0];
		  close (this->inPipe[0]);
		  this->inPipe[0] = -1;
		  forkStdin = true;
		}
	}
}

bool CGIGenerator::fullyHandled () const
{
  return handled;
}

void CGIGenerator::setUpArgv ()
{
  argv = new char *[3];
  argv[0] = new char[cgiPath.length () + 1];
  argv[1] = new char[filePath.length () + 1];
  ft::strcpy (argv[0], cgiPath.c_str ());
  ft::strcpy (argv[1], filePath.c_str ());
  argv[2] = NULL;

  if (g_vverbose)
	{
	  std::cout << "argv:" << std::endl;
	  std::cout << "\t" << argv[0] << std::endl;
	  std::cout << "\t" << argv[1] << std::endl;
	}
}

void CGIGenerator::setUpEnvp ()
{
  envStr = new std::string[FIELD_NB + request.getHeaders ().size ()];
  setUpEnvStr ();
  envp = new char *[FIELD_NB + request.getHeaders ().size () + 1];
  if (g_vverbose)
	std::cout << "envp:" << std::endl;
  for (size_t i (0); i < FIELD_NB + request.getHeaders ().size (); i++)
	{
	  envp[i] = new char[envStr[i].length () + 1];
	  ft::strcpy (envp[i], envStr[i].c_str ());
	  if (g_vverbose)
		std::cout << "\t" << envp[i] << std::endl;
	}
  envp[FIELD_NB + request.getHeaders ().size ()] = NULL;
}

void CGIGenerator::setUpEnvStr ()
{
  envStr[0] =
	  std::string ("AUTH_TYPE=") + request.get ("Authorization");
  char b[20];
  ft::itoa (b, request.getBody ().size ());
  envStr[1] = std::string ("CONTENT_LENGTH=") + b;
  envStr[2] =
	  std::string ("CONTENT_TYPE=") + request.get ("Content-Type");
  envStr[3] = "GATEWAY_INTERFACE=CGI/1.1"; // As by apache2 default
  envStr[4] = std::string ("PATH_INFO=") + request.getRLine ().target;
  envStr[5] = std::string ("PATH_TRANSLATED=") + filePath;
  envStr[6] = std::string ("QUERY_STRING=") + request.getQueryStr ();
  envStr[7] = std::string ("REMOTE_ADDR=127.0.0.1"); // Server
  envStr[8] = std::string ("REMOTE_IDENT=");
  envStr[9] = std::string ("REMOTE_USER=");
  envStr[10] = std::string ("REQUEST_METHOD=")
			   + strMethods[static_cast<int>(requestLine.method)];
  envStr[11] = std::string ("REQUEST_URI=") + requestLine.target;
  if (request.getQueryStr ().length () != 0)
	envStr[11] += std::string ("?") + request.getQueryStr ();
  envStr[12] = std::string ("SCRIPT_NAME=") + filePath;
  std::string host (request.get ("Host"));
  envStr[13] = std::string ("SERVER_NAME=")
			   + std::string (host, 0, host.find (":")); // ASsumption Host checked earlier
  //char buf[33];
  envStr[14] = std::string ("SERVER_PORT=")
			   + std::string (host, host.find (":") + 1); // to change
  envStr[15] = std::string ("SERVER_PROTOCOL=") + requestLine.httpVersion;
  envStr[16] = std::string ("SERVER_SOFTWARE=webserv/1.0");
  envStr[17] = std::string ("REDIRECT_STATUS=200"); // Refer to: http://php.net/security.cgi-bin
  int i (18);
  for (std::map<std::string, std::string>::const_iterator it (request.getHeaders ().begin ());
	   it != request.getHeaders ().end (); it++)
	{
	  envStr[i] = it->first;
	  std::replace (envStr[i].begin (), envStr[i].end (), '-', '_');
	  envStr[i].insert (0, "HTTP_", 0, 5);
	  std::transform (envStr[i].begin (), envStr[i].end (), envStr[i].begin (), ft::toupper);
	  envStr[i] += "=";
	  envStr[i] += it->second;
	  i++;
	}
}

void CGIGenerator::handleCGIHeaders ()
{
  std::string headersCGI;

  if (file.find ("\r\n\r\n") == std::string::npos)
	return;
  headersCGI = std::string (file, 0, file.find ("\r\n\r\n"));
  file.erase (0, headersCGI.length () + 4);
  while (headersCGI.length () != 0)
	{
	  // assumption CGI doesnt put mistakes in headers
	  if (std::string (headersCGI, 0, headersCGI.find (":")) != "Status")
		resp.set (std::string (headersCGI, 0, headersCGI.find (":")),
				  std::string (headersCGI, headersCGI.find (":") + 2,
							   headersCGI.find ("\n"))); // + 2 for ': '
	  if (headersCGI.find ("\n") != std::string::npos)
		headersCGI.erase (0, headersCGI.find ("\n") + 1);
	  else
		break;
	}
}

void CGIGenerator::destroy ()
{}

std::string CGIGenerator::genGetMime () const
{
  return "";
}
