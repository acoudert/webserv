#include "AutoIndexGenerator.hpp"
#include "pathUtils.hpp"
#include <cassert>
#include <sys/stat.h>
#include <cstdio>
#include <libft.hpp>

AutoIndexGenerator::AutoIndexGenerator (std::string const &_path, std::string const &reqUrl)
	: path (_path), dirFd (NULL), size (0), buf (AUTOINDEX_BUF)
{
  this->dirFd = opendir (path.c_str ());
  assert (this->dirFd != NULL);

  this->buf.append ((char *) ("<html>\n"
							  "<head><title>Index of "), 29);
  this->buf.append (reqUrl.c_str (), reqUrl.length ());
  this->buf.append ((char *) ("</title></head>\n"
							  "<body>\n"
							  "<h1>Index of "), 36);
  this->buf.append (reqUrl.c_str (), reqUrl.length ());
  this->buf.append ((char *) ("</h1><hr><pre>"), 14);
  for (struct dirent *dir = readdir (this->dirFd);
	   dir != NULL; dir = readdir (this->dirFd))
	{
	  assert (dir != NULL);
	  this->buf.append ((char *) ("<a href=\""), 9);
	  size_t l = ft::strlen (dir->d_name);
	  this->buf.append (dir->d_name, l);
	  this->buf.append ((char *) ("\">"), 2);
	  this->buf.append (dir->d_name, l);
	  this->buf.append ((char *) ("</a>"), 4);
	  while (l / 4 < 16)
		{
		  this->buf.append ((char *) ("\t"), 1);
		  l += 8;
		}
	  char tmp[8192];
	  struct tm t;
	  struct stat dent;
	  lstat (pathAppend (path, dir->d_name).c_str (), &dent);
	  localtime_r (&(dent.st_mtim.tv_sec), &t);
	  strftime (tmp, 8192, "%d-%b-%Y %H:%M", &t);
	  this->buf.append (tmp, ft::strlen (tmp));
	  this->buf.append ((char *) ("\t\t"), 2);
	  ft::itoa (tmp, dent.st_size);
	  this->buf.append (tmp, ft::strlen (tmp));
	  this->buf.append ((char *) ("\n"), 1);
	}
  this->buf.append ((char *) ("</pre><hr></body>\n"
							  "</html>"), 25);
  this->size = this->buf.used ();
  closedir (this->dirFd);
}

ssize_t
AutoIndexGenerator::Read (char *buf, size_t s, fd_set const *selectedFDs)
{
  size_t l = std::min (s, this->buf.used ());

  (void) selectedFDs;
  ft::memcpy (buf, this->buf.get (), l);
  this->buf.pop (l);
  return l;
}

ssize_t AutoIndexGenerator::getSize (fd_set const *selectedFDs, int *toCloseFds)
{
  (void) selectedFDs;
  (void) toCloseFds;
  return this->size;
}

IGenerator *AutoIndexGenerator::clone () const
{
  return new AutoIndexGenerator (*this);
}

AutoIndexGenerator::AutoIndexGenerator (const AutoIndexGenerator &x)
	: path (x.path), dirFd (x.dirFd), size (x.size), buf (x.buf)
{}

AutoIndexGenerator &AutoIndexGenerator::operator= (const AutoIndexGenerator &x)
{
  this->path = x.path;
  this->dirFd = x.dirFd;
  this->buf = x.buf;
  this->size = x.size;
  return *this;
}

bool AutoIndexGenerator::fullyHandled () const
{
  return true;
}

void AutoIndexGenerator::destroy ()
{}

std::string AutoIndexGenerator::genGetMime () const
{
  return "text/html";
}
