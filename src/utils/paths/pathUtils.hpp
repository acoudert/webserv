#ifndef _PATHUTILS_HPP_

#include <string>

std::string pathAppend (const std::string &path1, const std::string &path2);
std::string fileName (const std::string &path);
std::string parentDir (const std::string &path);
bool isPathSafe (std::string path);

#endif //_PATHUTILS_HPP_
