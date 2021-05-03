#ifndef _UTILS_HPP_
#define _UTILS_HPP_

#include <string>
#include <fstream>

enum Versions {
	HTTP_1_1,
	HTTP_1_0,
	UNKNOWN_VERSION,
};

std::string strip (std::string str, std::string const &c_to_strip);
void skipEmptyLines (std::ifstream &strStream, std::string &line);
bool isNumber (char const &c);
void removeComments (std::string &line);
size_t findIndexChar (std::string const &str, std::string const &toFind);

typedef unsigned int t_port;

void tvToStr (char buf[42], struct timeval *tv);

#endif // _UTILS_HPP_
