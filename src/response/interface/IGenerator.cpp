#include "IGenerator.hpp"

IGenerator::IGenerator ()
{
  this->forceStatus = 0;
}

void IGenerator::assignStatusCode (unsigned short status)
{
  this->forceStatus = status;
}

unsigned short IGenerator::updateStatusCode (unsigned short status) const
{
  return (this->forceStatus == 0 ? status : this->forceStatus);
}
