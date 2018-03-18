
#include "defs.h"
#include "FaceInfo.h"


// the following eyes, nose, chin ratios worked for a test face (i.e. mine)
// but they may need slight tweaks for other faces


const int iris_roi_div = 3;


FaceInfo::FaceInfo() :
    is_face_found(false),
    is_eyes_detect_enabled(true),
    is_grin_detect_enabled(true),
    smile_thr(10),
    eyes_ratio(0.2),
    nose_ratio(0.5625),
    grin_ratio(0.5625),
    chin_ratio(1.1),
    side_ratio(0.125)
{
}


FaceInfo::~FaceInfo()
{
}


void FaceInfo::reset_results()
{
    // clear all data that is based on classifier results
    
    is_face_found = false;
    
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
    rect_irisL_roi = cv::Rect();
    rect_irisR_roi = cv::Rect();
    rect_grin_roi = cv::Rect();
}


void FaceInfo::apply_face(const cv::Size& rImgSize, const cv::Rect& rFace)
{
    is_face_found = true;

    // determine coordinates for face feature regions
    int h_eyes = static_cast<int>((rFace.height) * eyes_ratio);
    int h_nose = static_cast<int>((rFace.height) * nose_ratio);
    int h_grin = static_cast<int>((rFace.height) * grin_ratio);
    int h_chin = static_cast<int>((rFace.height) * chin_ratio);
    int w_side = static_cast<int>((rFace.width) * side_ratio);

    x1 = rFace.x;
    y1 = rFace.y;
    x2 = rFace.x + rFace.width;
    xa = rFace.x + w_side;
    xb = rFace.x + rFace.width - w_side;
    xhalf = rFace.x + (rFace.width / 2);
    yeyes = rFace.y + h_eyes;
    ynose = rFace.y + h_nose;
    ygrin = rFace.y + h_grin;
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
    rect_grin_roi = cv::Rect(cv::Point(xa, ygrin), cv::Point(xb, ychin));
}


void FaceInfo::apply_eyeL(const cv::Rect& rEye)
{
    rect_eyeL = rEye;
    rect_eyeL.x += x1;
    rect_eyeL.y += yeyes;

    // search region for iris is a central box in the eye rectangle
    int xoffset = rEye.width / iris_roi_div;
    int yoffset = rEye.height / iris_roi_div;
    rect_irisL_roi = cv::Rect(
        cv::Point(rEye.x + xoffset, rEye.y + yoffset),
        cv::Point(rEye.x + rEye.width - xoffset, rEye.y + rEye.height - yoffset));
}


void FaceInfo::apply_eyeR(const cv::Rect& rEye)
{
    rect_eyeR = rEye;
    rect_eyeR.x += xhalf;
    rect_eyeR.y += yeyes;
    
    // search region for iris is a central box in the eye rectangle
    int xoffset = rEye.width / iris_roi_div;
    int yoffset = rEye.height / iris_roi_div;
    rect_irisR_roi = cv::Rect(
        cv::Point(rEye.x + xoffset, rEye.y + yoffset),
        cv::Point(rEye.x + rEye.width - xoffset, rEye.y + rEye.height - yoffset));
}


void FaceInfo::rgb_draw_boxes(cv::Mat& rImg) const
{
    // draw boxes around face features depending on mode
    // and whether face was found or not
    if (is_face_found)
    {
        if (is_grin_detect_enabled)
        {
            for (size_t j = 0; j < obj_grin.size(); j++)
            {
                rectangle(rImg, obj_grin[j], SCA_RED_MED);
            }
        }

        if (is_eyes_detect_enabled)
        {
            rectangle(rImg, rect_eyeL, SCA_GREEN);
            rectangle(rImg, rect_eyeR, SCA_GREEN);

            if (circlesL.size())
            {
                int xoffset = rect_eyeL.width / iris_roi_div;
                int yoffset = rect_eyeL.height / iris_roi_div;
                cv::Vec3i c = circlesL[0];
                cv::Point center = cv::Point(c[0] + rect_eyeL.x + xoffset, c[1] + rect_eyeL.y + yoffset);
                int radius = c[2];
                cv::circle(rImg, center, radius, cv::Scalar(255, 0, 255), 1, cv::LINE_AA);
            }

            if (circlesR.size())
            {
                int xoffset = rect_eyeR.width / iris_roi_div;
                int yoffset = rect_eyeR.height / iris_roi_div;
                cv::Vec3i c = circlesR[0];
                cv::Point center = cv::Point(c[0] + rect_eyeR.x + xoffset, c[1] + rect_eyeR.y + yoffset);
                int radius = c[2];
                cv::circle(rImg, center, radius, cv::Scalar(255, 0, 255), 1, cv::LINE_AA);
            }
        }

        cv::Scalar sca_face = SCA_CYAN;
        line(rImg, cv::Point(x1, yeyes), cv::Point(x2, yeyes), sca_face);
        line(rImg, cv::Point(x1, ynose), cv::Point(x2, ynose), sca_face);
        line(rImg, cv::Point(xhalf, y1), cv::Point(xhalf, ynose), sca_face);
        line(rImg, cv::Point(xa, ygrin), cv::Point(xb, ygrin), sca_face);
        line(rImg, cv::Point(xa, ygrin), cv::Point(xa, ychin - 1), sca_face);
        line(rImg, cv::Point(xb, ygrin), cv::Point(xb, ychin - 1), sca_face);
        rectangle(rImg, rect_face, sca_face);
    }
}
