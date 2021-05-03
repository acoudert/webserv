#include "pathUtils.hpp"
#include <cassert>
#include <cstring>

std::string pathAppend (const std::string &path1, const std::string &path2)
{
  if (path2.length () == 0)
	return path1;
  if (path1.length () == 0)
	{
	  if (path2[0] != '/')
		return "/" + path2;
	  return path2;
	}
  std::string retPath = path1;
  if (retPath[retPath.size () - 1] != '/')
	retPath += '/';

  if (path2[0] == '/')
	retPath.erase (retPath.end () - 1);

  retPath += path2;
  return retPath;
}

std::string fileName (const std::string &path)
{
  for (std::string::const_reverse_iterator it = path.rbegin ();
	   it != path.rend (); ++it)
	{
	  if (*it == '/')
		return path.substr (path.rend () - it);
	}
  return path;
}

std::string parentDir (const std::string &path)
{
  for (std::string::const_reverse_iterator it = path.rbegin ();
	   it != path.rend (); ++it)
	{
	  if (*it == '/')
		return path.substr (0, path.rend () - it);
	}
  return path;
}

bool isPathSafe (std::string path)
{
  if (path.length () == 0)
	return false;
  if (path[path.length () - 1] != '/')
	path += '/';
  if (path[0] != '/')
	return false;
  signed long long count = 0;
  for (size_t i = 0; i < path.length ();)
	{
	  size_t j;
	  for (j = i; j < path.length (); ++j)
		{
		  if (path[j] == '/' && j != i)
			break;
		}
	  ++i;
	  switch (j - i)
		{
		  case 1:
			if (path[i] != '.')
			  ++count;
		  break;
		  case 2:
			if (path[i] == '.' && path[i + 1] == '.')
			  {
				--count;
				break;
			  }
		  case 0:
			break;
		  default:
			++count;
		}
	  if (count < 0)
		return false;
	  i = j;
	}
  return true;
}
