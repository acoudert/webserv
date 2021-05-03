#ifndef _METHODS_HPP_
#define _METHODS_HPP_

#include <string>

#define METHODS_NB    5

typedef enum eMethods {
	GET,
	HEAD,
	POST,
	PUT,
	UNKNOWN_METHOD
} Methods;

// Used to set up Methods enum value
extern std::string const strMethods[METHODS_NB];

#endif
