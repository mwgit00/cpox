#ifndef UTIL_H_
#define UTIL_H_

#include <string>

namespace util
{
    /// Get Windows string resource by its ID
    std::string GetString(const int nIDS);
    
    /// Combine key character with Windows string resource
    std::string GetKeyHelpString(const char key, const int nIDS);
    
    /// Check if file path is valid
    bool IsPathOK(const std::string& rs);
}

#endif // UTIL_H_
