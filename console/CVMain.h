#ifndef CV_MAIN_H_
#define CV_MAIN_H_	

#include <string>

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

private:
    
    cv::CascadeClassifier cc_face;
    cv::CascadeClassifier cc_eyes;
    cv::CascadeClassifier cc_grin;
};

#endif // CV_MAIN_H_	
