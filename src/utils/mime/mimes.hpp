#ifndef _MIMES_HPP_
#define _MIMES_HPP_

#include <stddef.h>
#include <string>

extern char _binary_assets_mime_types_start[];
extern char _binary_assets_mime_types_end[];

struct mimeInfo {
	char *extBeg;
	char *extEnd;
	size_t extSize;
	char *mimeBeg;
	char *mimeEnd;
	size_t mimeSize;
};

extern mimeInfo *mimes;
extern size_t mimeCount;

void initMimes ();
std::string getMime (const char *fileName);

#endif //_MIMES_HPP_
