#include "strutil.h"

#include <string>
#include <vector>
#include <Windows.h>

std::wstring trimString(const std::wstring &input, int maxLen)
{
    if (input.size() <= maxLen)
    {
        return input;
    }

    return input.substr(0, maxLen - 1).append(L"...");
}

std::wstring convertToWide(const std::string &utf8Str)
{
    if (utf8Str.empty())
        return std::wstring();

    int wstrSize = MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, nullptr, 0);

    if (wstrSize == 0)
    {
        return std::wstring();
    }

    std::vector<wchar_t> buffer(wstrSize);
    MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, &buffer[0], wstrSize);

    return std::wstring(buffer.begin(), buffer.end() - 1); // -1 to exclude the null terminator
}