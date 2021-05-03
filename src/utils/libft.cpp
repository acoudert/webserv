#include <cstddef>
#include <stdint.h>
#include "libft.hpp"

namespace ft
{
	bool isspace (char I)
	{
	  return (I == ' ' || I == '\f' || I == '\n' || I == '\r' || I == '\t'
			  || I == '\v');
	}

	bool isdigit (char I)
	{
	  return I >= '0' && I <= '9';
	}

	void *memcpy (void *dst, void const *src, size_t len)
	{
	  unsigned char *pcDst = (unsigned char *) dst;
	  unsigned char const *pcSrc = (unsigned char const *) src;

	  if (len >= sizeof (long) * 2
		  && ((uintptr_t) src & (sizeof (long) - 1))
			 == ((uintptr_t) dst & (sizeof (long) - 1)))
		{
		  while (((uintptr_t) pcSrc & (sizeof (long) - 1)) != 0)
			{
			  *pcDst++ = *pcSrc++;
			  len--;
			}
		  long *plDst = (long *) pcDst;
		  long const *plSrc = (long const *) pcSrc;
		  /* manually unroll the loop */
		  while (len >= sizeof (long) * 4)
			{
			  plDst[0] = plSrc[0];
			  plDst[1] = plSrc[1];
			  plDst[2] = plSrc[2];
			  plDst[3] = plSrc[3];
			  plSrc += 4;
			  plDst += 4;
			  len -= sizeof (long) * 4;
			}
		  while (len >= sizeof (long))
			{
			  *plDst++ = *plSrc++;
			  len -= sizeof (long);
			}
		  pcDst = (unsigned char *) plDst;
		  pcSrc = (unsigned char const *) plSrc;
		}
	  while (len--)
		{
		  *pcDst++ = *pcSrc++;
		}
	  return dst;
	}

	void *memmove (void *dest, const void *src, size_t count)
	{
	  char *d = static_cast<char *>(dest);
	  const char *s = static_cast<char const *>(src);

	  if (s < d)
		{
		  s += count;
		  d += count;
		  while (count--)
			*--d = *--s;
		}
	  else
		{
		  while (count--)
			*d++ = *s++;
		}

	  return dest;
	}

	void strcpy (char *dest, const char *src)
	{
	  while (*src != '\0')
		*(dest++) = *(src)++;
	  *dest = *src;
	}

	size_t strlen (const char *ptr)
	{
	  const char *cpy = ptr;
	  while (*cpy != '\0')
		++cpy;
	  return cpy - ptr;
	}

	int toupper (int c)
	{
	  if (c >= 'a' && c <= 'z')
		return c - 32;
	  return c;
	}

	signed long long atoi (const char *nptr)
	{
	  signed long long res;
	  int sign;

	  while (isspace (*(nptr++)) == 1);
	  nptr--;
	  sign = *nptr == '-' ? -1 : 1;
	  nptr += *nptr == '-' || *nptr == '+' ? 1 : 0;
	  res = 0;
	  while (isdigit (*nptr))
		{
		  if (res > LLONG_MAX / 10 - sign * (*(nptr) - '0'))
			return -1;
		  res = res * 10 + sign * (*(nptr++) - '0');
		}
	  return (res);
	}

	int strncmp (const char *s1, const char *s2, size_t n)
	{
	  size_t i;

	  i = 0;
	  if (n == 0)
		return (0);
	  while (s1[i] != '\0' && s2[i] != '\0' && i < n)
		{
		  if ((unsigned char) s1[i] != (unsigned char) s2[i])
			break;
		  i++;
		}
	  if (i == n)
		return (0);
	  return ((unsigned char) s1[i] - (unsigned char) s2[i]);
	}

}
