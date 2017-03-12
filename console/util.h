#ifndef UTIL_H_
#define UTIL_H_

#include <string>

namespace util
{
    std::string GetString(const int nIDS);
    std::string GetKeyHelpString(const char key, const int nIDS);
    bool IsPathOK(const std::string& rs);
}

#endif // UTIL_H_
