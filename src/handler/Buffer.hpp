#ifndef _BUFFER_HPP_
#define _BUFFER_HPP_

#include <cstring>

#define REQ_BUF_SIZE 1048576
#define COMMON_BUF_ASSERT assert(this->end >= this->begin);

class Buffer {
 private:
  size_t cap;
  size_t begin;
  size_t end;
  char *buf;
 public:
  explicit Buffer (size_t capacity);
  Buffer &operator= (const Buffer &x);
  ~Buffer ();

  explicit Buffer (const Buffer &x);
  const char *get () const;
  void pop (size_t n);
  void push (const char *buf, size_t n);
  void append (const char *buf, size_t n);
  size_t used () const;
  size_t available () const;
  void reserve (size_t s);
};

#endif // _BUFFER_HPP_
