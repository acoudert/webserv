#include "EmptyGenerator.hpp"
#include <cassert>

ssize_t EmptyGenerator::Read (char *buf, size_t s, fd_set const *selectedFDs)
{
  (void) buf;
  (void) s;
  (void) selectedFDs;
  return 0;
}

ssize_t EmptyGenerator::getSize (fd_set const *selectedFDs, int *toCloseFds)
{
  (void) selectedFDs;
  (void) toCloseFds;
  return 0;
}

IGenerator *EmptyGenerator::clone () const
{
  return new EmptyGenerator ();
}

EmptyGenerator::EmptyGenerator ()
{}

bool EmptyGenerator::fullyHandled () const
{
  return true;
}

void EmptyGenerator::destroy ()
{}
std::string EmptyGenerator::genGetMime () const
{
  return "";
}
