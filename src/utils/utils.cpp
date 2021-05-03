#include "utils.hpp"
#include "libft.hpp"

// Remove all char from c_to_strip at the beginning and at the end of str
std::string strip (std::string str, std::string const &c_to_strip)
{
  std::string::iterator it (str.begin ());

  while (c_to_strip.find (*it) != std::string::npos)
	++it;
  str.erase (str.begin (), it);
  it = str.end ();
  --it;
  while (it != str.begin () && c_to_strip.find (*it) != std::string::npos)
	--it;
  if (c_to_strip.find (*it) == std::string::npos)
	++it;
  str.erase (it, str.end ());
  return str;
}

#include <iostream>
#include <cstring>
// Skip over empty lines and update istringstream accordingly
void skipEmptyLines (std::ifstream &file, std::string &line)
{
  removeComments (line);
  line = strip (line, " \t");
  while (!file.eof () && line.length () <= 0)
	{
	  std::getline (file, line);
	  removeComments (line);
	  line = strip (line, " \t");
	}
}

bool isNumber (char const &c)
{
  if (c >= '0' && c <= '9')
	return true;
  return false;
}

void removeComments (std::string &line)
{
  size_t pos (line.find ("#"));

  if (pos != std::string::npos)
	line.erase (pos);
}

size_t findIndexChar (std::string const &str, std::string const &toFind)
{
  for (size_t i (0); i < str.length (); i++)
	{
	  for (size_t j (0); j < toFind.length (); j++)
		{
		  if (str[i] == toFind[j])
			return i;
		}
	}
  return str.length ();
}

void tvToStr (char buf[42], struct timeval *tv)
{
  struct tm tM;
  localtime_r (&(tv->tv_sec), &tM);
  strftime (buf, 42, "@@@, @@ @@@ %Y %H:%M:%S GMT", &tM);
  buf[5] = tM.tm_mday / 10 + '0';
  buf[6] = tM.tm_mday % 10 + '0';
  char *days[] = {(char *) ("Mon"), (char *) ("Tue"), (char *) ("Wed"),
				  (char *) ("Thu"), (char *) ("Fri"), (char *) ("Sat"),
				  (char *) ("Sun")};
  char *months[] = {(char *) ("Jan"), (char *) ("Feb"), (char *) ("Mar"),
					(char *) ("Apr"), (char *) ("May"), (char *) ("Jun"),
					(char *) ("Jul"), (char *) ("Aug"), (char *) ("Sep"),
					(char *) ("Oct"), (char *) ("Nov"),
					(char *) ("Dec")};
  ft::memcpy (buf, days[tM.tm_wday], 3);
  ft::memcpy (&buf[8], months[tM.tm_mon], 3);
}
