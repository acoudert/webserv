#include "Buffer.hpp"
#include <cassert>
#include <libft.hpp>

Buffer::Buffer (size_t capacity)
	: cap (capacity), begin (0), end (0), buf (new char[capacity + 1])
{}

Buffer::Buffer (const Buffer &x)
	: cap (x.cap), begin (0), end (
	x.end - x.begin), buf (new char[x.cap + 1])
{
  COMMON_BUF_ASSERT
  ft::memcpy (this->buf, &x.buf[x.begin], this->end);
  this->buf[this->end] = 0x80;
  COMMON_BUF_ASSERT
}

// Return start address from Buffer
const char *Buffer::get () const
{
  COMMON_BUF_ASSERT
  return &this->buf[this->begin];
}

// Update Buffer array beginning index
// If all char already taken into account => put beginning index and end index at 0
void Buffer::pop (size_t n)
{
  COMMON_BUF_ASSERT
  this->begin += n;
  if (this->used () == 0)
	{
	  this->begin = 0;
	  this->end = 0;
	}
  COMMON_BUF_ASSERT
}

#include <iostream>
// Check whether or not Buffer can contain buf
// Update Buffer begining index if required and add buf into Buffer
void Buffer::push (const char *buf, size_t n)
{
  assert(this->used () + n <= this->cap);
  COMMON_BUF_ASSERT
  if (this->cap - n >= this->end)
	{
	  ft::memcpy (&this->buf[this->end], buf, n);
	  this->end += n;
	}
  else
	{
	  ft::memmove (this->buf, reinterpret_cast<void *>(&this->buf[this->begin]),
				   this->end - this->begin);
	  this->end = this->end - this->begin;
	  this->begin = 0;
	  this->push (buf, n);
	}
  this->buf[this->end] = 0x80;
  COMMON_BUF_ASSERT
}

// Return storage size unavailable in Buffer
size_t Buffer::used () const
{
  COMMON_BUF_ASSERT
  return this->end - this->begin;
}

// Return storage size still available in Buffer
size_t Buffer::available () const
{
  COMMON_BUF_ASSERT
  return this->cap - this->used ();
}

Buffer::~Buffer ()
{
  COMMON_BUF_ASSERT
  delete[] this->buf;
}
Buffer &Buffer::operator= (const Buffer &x)
{
  this->cap = x.cap;
  this->begin = 0;
  this->end = x.end - x.begin;
  ft::memcpy (this->buf, &x.buf[x.begin], this->end);
  COMMON_BUF_ASSERT
  return *this;
}

void Buffer::reserve (size_t s)
{
  COMMON_BUF_ASSERT
  if (this->available () > s)
	return;
  char *n = new char[s + this->used () + 1];
  ft::memcpy (n, &this->buf[this->begin], this->end - this->begin);
  this->cap = s + this->used ();
  delete[] this->buf;
  this->buf = n;
  this->end = this->end - this->begin;
  this->buf[this->end] = 0x80;
  this->begin = 0;
  COMMON_BUF_ASSERT
}

void Buffer::append (const char *buf, size_t n)
{
  COMMON_BUF_ASSERT
  while (this->available () < n)
	{
	  char *newBuf = new char[this->cap * 2 + 1];
	  ft::memcpy (newBuf, &this->buf[this->begin], this->used ());
	  delete[] this->buf;
	  this->buf = newBuf;
	  this->cap *= 2;
	  this->end = this->used ();
	  this->begin = 0;
	  COMMON_BUF_ASSERT
	}
  COMMON_BUF_ASSERT
  this->push (buf, n);
}
