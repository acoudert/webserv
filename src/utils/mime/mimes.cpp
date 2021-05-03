#include <cstdio>
#include <cassert>
#include <iostream>
#include <cstdlib>
#include "mimes.hpp"
#include "libft.hpp"

mimeInfo *mimes = NULL;
size_t mimeCount = 0;

void initMimes ()
{
  assert(mimes == NULL && mimeCount == 0);
  for (char *c = _binary_assets_mime_types_start;
	   c < _binary_assets_mime_types_end; ++c)
	{
	  ++mimeCount;
	  if (c == _binary_assets_mime_types_end)
		break;
	  bool b = false;
	  while (*c != ' ' && c < _binary_assets_mime_types_end)
		{
		  ++c;
		  b = true;
		}
	  if (!b)
		{
		  std::cout << "Invalid mime file" << std::endl;
		  exit (1);
		}
	  ++c;
	  for (b = false; *c != '\n' && c < _binary_assets_mime_types_end; ++c)
		{
		  if (*c == ' ')
			{
			  std::cout << "Invalid mime file" << std::endl;
			  exit (1);
			}
		  b = true;
		}
	  if (!b)
		{
		  std::cout << "Invalid mime file" << std::endl;
		  exit (1);
		}
	}
  mimes = new mimeInfo[mimeCount];
  size_t i = 0;
  for (char *c = _binary_assets_mime_types_start;
	   c < _binary_assets_mime_types_end; ++c)
	{
	  mimes[i].extBeg = c;
	  while (*c != ' ')
		++c;
	  mimes[i].extEnd = c;
	  mimes[i].mimeBeg = ++c;
	  while (*c != '\n')
		++c;
	  mimes[i].mimeEnd = c;
	  mimes[i].extSize = mimes[i].extEnd - mimes[i].extBeg;
	  mimes[i].mimeSize = mimes[i].mimeEnd - mimes[i].mimeBeg;
	  ++i;
	}
}

static std::string
dicho (mimeInfo *arr, size_t arrSize, const char *strBeg, const char *strEnd)
{
  (void) strEnd;
  size_t i = arrSize;
  while (i-- > 0)
	{
	  if (ft::strncmp (strBeg, arr[i].extBeg, arr[i].extEnd - arr[i].extBeg)
		  == 0)
		return std::string (arr[i].mimeBeg, arr[i].mimeSize);
	}
  return "application/octet-stream";
}

std::string getMime (const char *fileName)
{
  assert(mimes != NULL && mimeCount != 0);
  const char *ptr = fileName;
  const char *dot = NULL;
  while (*ptr != '\0')
	{
	  if (*ptr == '.')
		dot = ptr;
	  ++ptr;
	}
  if (dot == NULL)
	return "application/octet-stram";
  ++dot;
  return dicho (mimes, mimeCount, dot, ptr);
}
