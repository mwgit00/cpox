#include "Windows.h"
#include "resource.h"
#include "Shlwapi.h"


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


    ///@TODO -- port this old python code to C++ someday

    /*

    void AppMain::make_movie(img_path):
    """Generate an MOV file from a directory of PNG files."""

    // gather file names of frames
    img_files = []
    for (dirpath, dirnames, filenames) in os.walk(img_path):
    img_files.extend(filenames)
    break

    // only consider PNG files
    img_files = [each for each in img_files if each.rfind(".png") > 0]
    if len(img_files) == 0:
    std::cout << "No PNG files found!"
    return

    // determine size of frames
    file_path = os.path.join(img_path, img_files[0])
    size = cv.GetSize(cv.LoadImage(file_path))

    // build movie from separate frames
    // TODO -- may need to change FPS on different systems
    fps = 15
    movie_path = os.path.join(img_path, "movie.mov")
    video_maker = VideoWriter(movie_path,
    cv.CV_FOURCC('m', 'p', '4', 'v'),
    fps, size)
    if video_maker.isOpened()
    for each in img_files:
    file_path = os.path.join(img_path, each)
    img = imread(file_path)
    video_maker.write(img)
    */

}
