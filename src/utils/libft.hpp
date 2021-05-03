#ifndef _LIBFT_HPP_
#define _LIBFT_HPP_

#include <climits>

namespace ft
{
	template<typename T>
	T abs (const T &a)
	{
	  if (a < 0)
		return -a;
	  return a;
	}

	bool isspace (char I);
	bool isdigit (char I);

	signed long long atoi (const char *nptr);

	void *memcpy (void *dst, void const *src, std::size_t len);
	void *memmove (void *dest, const void *src, std::size_t count);

	template<typename T>
	static std::size_t chrN (T n)
	{
	  std::size_t i = 0;
	  goto loop;
	  while (n != 0)
		{
		loop:
		n /= 10;
		  ++i;
		}
	  return i;
	}

	template<typename T>
	void itoa (char *dest, T src)
	{
	  dest += chrN (src);
	  *(dest--) = '\0';
	  goto loop;
	  while (src != 0)
		{
		loop:
		*(dest--) = src % 10 + '0';
		  src /= 10;
		}
	}

	void strcpy (char *dest, const char *src);
	std::size_t strlen (const char *ptr);

	int toupper (int c);
	int strncmp (const char *s1, const char *s2, std::size_t n);

}

#endif //_LIBFT_HPP_
