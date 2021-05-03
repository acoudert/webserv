#include "error.hpp"

int error (std::string const &err, int const &errCode)
{
  std::cerr << "Error: " << err << std::endl;
  return errCode;
}
