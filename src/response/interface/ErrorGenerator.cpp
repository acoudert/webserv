#include "ErrorGenerator.hpp"
#include <cassert>
#include <cstring>
#include <libft.hpp>

ErrorGenerator::ErrorGenerator (const ErrorGenerator &x)
	: begin (x.begin), end (x.end), size (x.size)
{}

ErrorGenerator &ErrorGenerator::operator= (const ErrorGenerator &x)
{
  this->begin = x.begin;
  this->end = x.end;
  this->size = x.size;
  return *this;
}

ssize_t ErrorGenerator::Read (char *buf, size_t s, fd_set const *selectedFDs)
{
  size_t l = std::min (s, size_t (this->end - this->begin));

  (void) selectedFDs;
  ft::memcpy (buf, this->begin, l);
  this->begin += l;
  return l;
}

ssize_t ErrorGenerator::getSize (fd_set const *selectedFDs, int *toCloseFds)
{
  (void) selectedFDs;
  (void) toCloseFds;
  return this->size;
}

IGenerator *ErrorGenerator::clone () const
{
  return new ErrorGenerator (*this);
}

const struct errMapper mapperList[] = {
	{200, _binary_assets_200_html_start, _binary_assets_200_html_end},
	{201, _binary_assets_201_html_start, _binary_assets_201_html_end},
	{301, _binary_assets_301_html_start, _binary_assets_301_html_end},
	{400, _binary_assets_400_html_start, _binary_assets_400_html_end},
	{401, _binary_assets_401_html_start, _binary_assets_401_html_end},
	{403, _binary_assets_403_html_start, _binary_assets_403_html_end},
	{404, _binary_assets_404_html_start, _binary_assets_404_html_end},
	{405, _binary_assets_405_html_start, _binary_assets_405_html_end},
	{408, _binary_assets_408_html_start, _binary_assets_408_html_end},
	{413, _binary_assets_413_html_start, _binary_assets_413_html_end},
	{418, _binary_assets_418_html_start, _binary_assets_418_html_end},
	{500, _binary_assets_500_html_start, _binary_assets_500_html_end},
	{501, _binary_assets_501_html_start, _binary_assets_501_html_end},
	{505, _binary_assets_505_html_start, _binary_assets_505_html_end},
};

ErrorGenerator::ErrorGenerator (int code)
	: begin (NULL), end (NULL), size (0)
{
  for (unsigned int i = 0; i < sizeof (mapperList) / sizeof (*mapperList); ++i)
	{
	  if (mapperList[i].code == code)
		{
		  this->begin = mapperList[i].begin;
		  this->end = mapperList[i].end;
		  this->size = this->end - this->begin;
		  return;
		}
	}
  assert (false);
}

bool ErrorGenerator::fullyHandled () const
{
  return this->begin >= this->end;
}

void ErrorGenerator::destroy ()
{}

std::string ErrorGenerator::genGetMime () const
{
  return "text/html";
}

