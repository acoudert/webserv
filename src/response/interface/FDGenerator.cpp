#include "FDGenerator.hpp"

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <cstring>
#include <cerrno>
#include <sys/types.h>
#include <sys/stat.h>
#include <mimes.hpp>

FDGenerator::FDGenerator (int const &_fd, std::string const &_path)
	: path (_path), fd (_fd), bytesRead (0)
{}

FDGenerator::FDGenerator (const FDGenerator &x)
{
  *this = x;
}

FDGenerator &FDGenerator::operator= (const FDGenerator &x)
{
  this->path = x.path;
  this->fd = x.fd;
  bytesRead = x.bytesRead;
  return *this;
}

// Return buf based on what it could read from fd stored in FDGenerator
ssize_t FDGenerator::Read (char *buf, size_t s, fd_set const *selectedFDs)
{
  ssize_t res (0);

  if (FD_ISSET(fd, &selectedFDs[0]))
	{
	  //fixme if read failure
	  res = read (this->fd, buf, s);
	  if (res < 0)
		{
		  std::cout << "FDGenerator read error: " << strerror (errno)
					<< std::endl;
		  return 0;
		}
	  bytesRead += res;
	}
  return res;
}

ssize_t FDGenerator::getSize (fd_set const *selectedFDs, int *toCloseFds)
{
  struct stat st;

  (void) selectedFDs;
  (void) toCloseFds;
  lstat (this->path.c_str (), &st);
  return st.st_size;
}

IGenerator *FDGenerator::clone () const
{
  return new FDGenerator (*this);
}

FDGenerator::~FDGenerator ()
{}

bool FDGenerator::fullyHandled () const
{
  struct stat st;

  lstat (this->path.c_str (), &st);
  return st.st_size == bytesRead;
}

void FDGenerator::destroy ()
{
  if (this->fd != -1)
	close (fd);
}

std::string FDGenerator::genGetMime () const
{
  return getMime (this->path.c_str ());
}
