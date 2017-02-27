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
	void apply_face(const cv::Size& rSize, const cv::Rect& rRect);
	void rgb_draw_boxes(cv::Mat& rImg) const;

public:

	bool is_grin_enabled;

	double nose_ratio;
	double chin_ratio;

	int x1;
	int y1;
	int x2;
	int xhalf;
	int ynose;
	int ychin;

	cv::Rect rect_face;		///< rectangle for found face
	cv::Rect rect_eyeL;		///< rectangle for found left eye
	cv::Rect rect_eyeR;		///< rectangle for found right eye

	std::vector<cv::Rect> obj_grin;

	cv::Rect rect_eyeL_roi;	///< region-of-interest for left eye search
	cv::Rect rect_eyeR_roi;	///< region-of-interest for right eye search
	cv::Rect rect_grin_roi;	///< region-of-interest for grin search
};

#endif // FACE_INFO_H_
