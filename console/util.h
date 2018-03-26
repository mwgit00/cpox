#ifndef UTIL_H_
#define UTIL_H_

#include <string>
#include <list>

namespace util
{
    /// Get Windows string resource by its ID
    std::string GetString(const int nIDS);
    
    /// Combine key character with Windows string resource
    std::string GetKeyHelpString(const char key, const int nIDS);
    
    /// Check if file path is valid
    bool IsPathOK(const std::string& rs);

    /// Get list of all files in a directory that match a pattern
    void GetListOfFiles(
        const std::string& rsdir,
        const std::string& rspattern,
        std::list<std::string>& listOfFiles);
}

#endif // UTIL_H_
