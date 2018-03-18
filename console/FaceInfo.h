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

    std::vector<cv::Vec3f>& get_circlesL() { return circlesL; }
    std::vector<cv::Vec3f>& get_circlesR() { return circlesR; }

public:

    bool is_face_found;
    bool is_eyes_detect_enabled;
    bool is_grin_detect_enabled;

    int smile_thr;

    double eyes_ratio;
    double nose_ratio;
    double grin_ratio;
    double chin_ratio;
    double side_ratio;

    int x1;     ///< top-left x-coord
    int y1;     ///< top-left y-coord
    int x2;     ///< top-right x-coord
    int xa;     ///< "indented" left side
    int xb;     ///< "indented" right side
    int xhalf;  ///< divides face in half
    int yeyes;  ///< uppermost y-coord for eye search
    int ynose;  ///< lowermost y-coord for eye search
    int ygrin;  ///< uppermost y-coord for grin search
    int ychin;  ///< lowermost y-coord for grin search (may extend below face)

    cv::Rect rect_face; ///< rectangle for found face
    cv::Rect rect_eyeL; ///< rectangle for found left eye
    cv::Rect rect_eyeR; ///< rectangle for found right eye

    std::vector<cv::Vec3f> circlesL;
    std::vector<cv::Vec3f> circlesR;

    std::vector<cv::Rect> obj_grin; ///< all grins that have been found

    cv::Rect rect_eyeL_roi;	    ///< region-of-interest for left eye search
    cv::Rect rect_eyeR_roi;	    ///< region-of-interest for right eye search
    cv::Rect rect_irisL_roi;	///< region-of-interest for left iris search
    cv::Rect rect_irisR_roi;    ///< region-of-interest for right iris search
    cv::Rect rect_grin_roi;	    ///< region-of-interest for grin search
};

#endif // FACE_INFO_H_
