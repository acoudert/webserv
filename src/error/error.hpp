#ifndef _ERROR_HPP_
#define _ERROR_HPP_

#include <iostream>
#include <cstring>
#include <cerrno>

#define SERVER_CONF            1
#define SOCKET                    2

int error (std::string const &err, int const &errCode);

#endif // _ERROR_HPP_
