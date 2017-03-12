
#include "defs.h"
#include "FaceInfo.h"


// the following eyes, nose, chin ratios worked for a test face (i.e. mine)
// but they may need slight tweaks for other faces


FaceInfo::FaceInfo() :
    is_eyes_enabled(true),
    is_grin_enabled(true),
    eyes_ratio(0.2),
    nose_ratio(0.5625),
    chin_ratio(1.125)
{
}


FaceInfo::~FaceInfo()
{
}


void FaceInfo::reset_results()
{
    // clear all data that is based on classifier results
    
    x1 = 0;
    y1 = 0;
    x2 = 0;
    xhalf = 0;
    ynose = 0;
    ychin = 0;
    
    rect_face = cv::Rect();
    rect_eyeL = cv::Rect();
    rect_eyeR = cv::Rect();
    
    obj_grin.clear();

    rect_eyeL_roi = cv::Rect();
    rect_eyeR_roi = cv::Rect();
    rect_grin_roi = cv::Rect();
}


void FaceInfo::apply_face(const cv::Size& rImgSize, const cv::Rect& rFace)
{
    // determine coordinates for face feature regions
    int h_eyes = static_cast<int>((rFace.height) * eyes_ratio);
    int h_nose = static_cast<int>((rFace.height) * nose_ratio);
    int h_chin = static_cast<int>((rFace.height) * chin_ratio);

    x1 = rFace.x;
    y1 = rFace.y;
    x2 = rFace.x + rFace.width;
    xhalf = rFace.x + (rFace.width / 2);
    yeyes = rFace.y + h_eyes;
    ynose = rFace.y + h_nose;
    ychin = rFace.y + h_chin;

    // y-coord for chin may go out of bounds so adjust if necessary
    if (ychin > rImgSize.height)
    {
        ychin = rImgSize.height;
    }

    // set rectangle for found face
    rect_face = cv::Rect(cv::Point(x1, y1), cv::Point(x2, ychin));

    // assign eye search rectangles ("left" is left side of displayed image)
    rect_eyeL_roi = cv::Rect(cv::Point(x1, yeyes), cv::Point(xhalf, ynose));
    rect_eyeR_roi = cv::Rect(cv::Point(xhalf, yeyes), cv::Point(x2, ynose));

    // assign grin search rectangle
    rect_grin_roi = cv::Rect(cv::Point(x1, ynose), cv::Point(x2, ychin));
}


void FaceInfo::apply_eyeL(const cv::Rect& rEye)
{
    rect_eyeL = rEye;
    rect_eyeL.x += x1;
    rect_eyeL.y += yeyes;
}


void FaceInfo::apply_eyeR(const cv::Rect& rEye)
{
    rect_eyeR = rEye;
    rect_eyeR.x += xhalf;
    rect_eyeR.y += yeyes;
}


void FaceInfo::rgb_draw_boxes(cv::Mat& rImg) const
{
    // draw face features

    if (is_grin_enabled)
    {
        for (size_t j = 0; j < obj_grin.size(); j++)
        {
            rectangle(rImg, obj_grin[j], SCA_RED_MED);
        }
    }

    if (is_eyes_enabled)
    {
        rectangle(rImg, rect_eyeL, SCA_GREEN);
        rectangle(rImg, rect_eyeR, SCA_GREEN);
    }

    cv::Scalar sca_face = SCA_CYAN;
    line(rImg, cv::Point(x1, yeyes), cv::Point(x2, yeyes), sca_face);
    line(rImg, cv::Point(x1, ynose), cv::Point(x2, ynose), sca_face);
    line(rImg, cv::Point(xhalf, y1), cv::Point(xhalf, ynose), sca_face);
    rectangle(rImg, rect_face, sca_face);
}
