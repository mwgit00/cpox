#ifndef FACE_INFO_H_
#define FACE_INFO_H_

#include <vector>
#include "opencv2/imgproc/imgproc.hpp"

class FaceInfo
{
public:
    
    FaceInfo();
    virtual ~FaceInfo();

    void reset_results();
    void apply_face(const cv::Size& rImgSize, const cv::Rect& rRect);
    void apply_eyeL(const cv::Rect& rEye);
    void apply_eyeR(const cv::Rect& rEye);
    void rgb_draw_boxes(cv::Mat& rImg) const;

public:

    bool is_eyes_enabled;
    bool is_grin_enabled;

    double nose_ratio;
    double chin_ratio;

    int x1;     ///< top-left x-coord
    int y1;     ///< top-left y-coord
    int x2;     ///< top-right x-coord
    int xhalf;  ///< divides face in half
    int ynose;  ///< y-coord for horizontal nose line
    int ychin;  ///< y-coord for chin line

    cv::Rect rect_face; ///< rectangle for found face
    cv::Rect rect_eyeL; ///< rectangle for found left eye
    cv::Rect rect_eyeR; ///< rectangle for found right eye

    std::vector<cv::Rect> obj_grin;

    cv::Rect rect_eyeL_roi;	///< region-of-interest for left eye search
    cv::Rect rect_eyeR_roi;	///< region-of-interest for right eye search
    cv::Rect rect_grin_roi;	///< region-of-interest for grin search
};

#endif // FACE_INFO_H_
