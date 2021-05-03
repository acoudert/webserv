#ifndef _STATUSCODE_HPP_
#define _STATUSCODE_HPP_

#include <string>

#define MAXHEADER_LEN        8192
#define STATUSCODE_NB        6

#define OK                             200
#define MOVED_PERMANENTLY              301
#define BAD_REQUEST                    400
#define FORBIDDEN                      403
#define NOT_FOUND                      404
#define NOT_ALLOWED                    405
#define PAYLOAD_TOO_LARGE              413
#define URI_TOO_LONG                   414
#define INTERNAL_SERVER_ERROR          500
#define NOT_IMPLEMENTED                501

struct StatusCodes {
	short code;
	std::string codeStr;
};

#endif // _STATUSCODE_HPP_
