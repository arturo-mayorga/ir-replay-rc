#ifndef _strutil_h
#define _strutil_h

#include <string>

std::wstring trimString(const std::wstring &input, int maxLen);
std::wstring convertToWide(const std::string &utf8Str);

#endif