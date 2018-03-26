#include "Windows.h"
#include "resource.h"
#include "Shlwapi.h"

#include <list>
#include <iostream>
#include <sstream>
#include "util.h"

namespace util
{
    std::string GetString(const int nIDS)
    {
        const int STR_MAX_SIZE = 256;
        static TCHAR buff[STR_MAX_SIZE];

        ::LoadString(::GetModuleHandle(NULL), nIDS, buff, STR_MAX_SIZE);
        std::wstring ws(buff);

        return std::string(ws.begin(), ws.end());
    }

    std::string GetKeyHelpString(const char key, const int nIDS)
    {
        std::ostringstream oss;
        oss << key << " - " << GetString(nIDS) << ".";
        return oss.str();
    }

    bool IsPathOK(const std::string& rs)
    {
        std::wstring ws(rs.begin(), rs.end());
        BOOL result = ::PathFileExists(ws.c_str());
        return (result) ? true : false;
    }

    void GetListOfFiles(
        const std::string& rsdir,
        const std::string& rspattern,
        std::list<std::string>& listOfFiles)
    {
        std::string s = rsdir + "\\" + rspattern;

        WIN32_FIND_DATA search_data;
        memset(&search_data, 0, sizeof(WIN32_FIND_DATA));

        std::wstring ws(s.begin(), s.end());
        HANDLE handle = FindFirstFile(ws.data(), &search_data);

        while (handle != INVALID_HANDLE_VALUE)
        {
            std::wstring wsfile(search_data.cFileName);
            std::string sfile(wsfile.begin(), wsfile.end());
            listOfFiles.push_back(rsdir + "\\" + sfile);
            if (FindNextFile(handle, &search_data) == FALSE)
            {
                break;
            }
        }

        FindClose(handle);
    }
}
