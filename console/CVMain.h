#ifndef CV_MAIN_H_
#define CV_MAIN_H_	

#include <string>
#include <list>

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/objdetect/objdetect.hpp"

#include "FaceInfo.h"


class CVMain
{
public:
    
    CVMain();
    virtual ~CVMain();

    bool load_cascades(const std::string& path);
    bool detect(cv::Mat& r, FaceInfo& rFaceInfo);
    
    static void make_movie(const double fps, const std::string& rspath, const std::list<std::string>& rListOfPNG);

private:
    
    cv::CascadeClassifier cc_face;
    cv::CascadeClassifier cc_eyes;
    cv::CascadeClassifier cc_grin;
};

#endif // CV_MAIN_H_	
