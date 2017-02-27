// console.cpp : Defines the entry point for the console application.
//
// Face and Eye Finder
// With Eye Miss Detection
//

#include "stdafx.h"

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/objdetect/objdetect.hpp"

#include <iostream>
#include <string>
#include <iomanip>

#include "defs.h"
#include "FaceInfo.h"
#include "MissedDetectionTrigger.h"

using namespace std;
using namespace cv;


CascadeClassifier cc_face;
CascadeClassifier cc_eyes;
CascadeClassifier cc_grin;

const int EYE_LOST_CT = 30;
const int EYE_WARN_CT = 15;
const int RUNTIME_INH = 30;
const int STARTUP_INH = 20;


// global eye bounce object
MissedDetectionTrigger ebx;

String haar_cascade_path = "C:\\opencv-3.2.0\\opencv\\build\\etc\\haarcascades\\";
String path = "~/work/cpox/";
String pathx = "~/work/cpox/movie/";


void detect(const Mat& r, FaceInfo& rFaceInfo)
{
	std::vector<Rect> obj_face;
	std::vector<Rect> obj_eyeL;
	std::vector<Rect> obj_eyeR;

	const double scale_factor = 1.1;
	const int face_neighbor_ct = 2;
	const int eye_neighbor_ct = 4;

	// demo code does histogram equalization so do the same
	equalizeHist(r, r);

	// assume face will be "big"
	Size size_face(60, 60);

	// eyes will be smaller than face
	Size size_eyes(18, 18);

	// blow away old info
	rFaceInfo.reset_results();

	// assume eyes found
	// this will be applied to debounce
	// if face or eye detections fail (rare)
	bool bFound = false;

	// first find ONE face
	cc_face.detectMultiScale(r, obj_face, scale_factor, face_neighbor_ct, 0, size_face);
	if (obj_face.size() == 1)
	{
		const Rect& face = obj_face[0];
		rFaceInfo.apply_face(r.size(), obj_face[0]);

		Mat eyeL_ROI = r(rFaceInfo.rect_eyeL_roi);
		Mat eyeR_ROI = r(rFaceInfo.rect_eyeR_roi);

		// then search for eyes in eye rectangles
		// set found flag even if only one is found
	
		cc_eyes.detectMultiScale(eyeL_ROI, obj_eyeL, scale_factor, eye_neighbor_ct, 0, size_eyes);
		if (obj_eyeL.size() >= 1)
		{
			rFaceInfo.rect_eyeL = obj_eyeL[0];
			rFaceInfo.rect_eyeL.x += rFaceInfo.x1;
			rFaceInfo.rect_eyeL.y += rFaceInfo.y1;
			bFound = true;
		}

		cc_eyes.detectMultiScale(eyeR_ROI, obj_eyeR, scale_factor, eye_neighbor_ct, 0, size_eyes);
		if (obj_eyeR.size() >= 1)
		{
			rFaceInfo.rect_eyeR = obj_eyeR[0];
			rFaceInfo.rect_eyeR.x += rFaceInfo.xhalf;
			rFaceInfo.rect_eyeR.y += rFaceInfo.y1;
			bFound = true;
		}

		if (true)
		{
			// try to find grin in mouth area
			// use magic parameter to tune
			// TODO:  tune during start-up?

			int magic = 0;
			
			Mat grinROI = r(rFaceInfo.rect_grin_roi);
			int w = (rFaceInfo.rect_grin_roi.width * 3) / 8;  // min 3/8 of mouth region width
			int h = (rFaceInfo.rect_grin_roi.height * 1) / 3;  // min 1/3 of mouth region height
			cc_grin.detectMultiScale(grinROI, rFaceInfo.obj_grin, 1.1, magic, 0, Size(w, h));

			// this statement controls whether or not grin
			// is required to determine if face "eye" state is okay
			bFound = bFound && (rFaceInfo.obj_grin.size() > 0);

			// offset the grin boxes
			for (size_t j = 0; j < rFaceInfo.obj_grin.size(); j++)
			{
				rFaceInfo.obj_grin[j].x += rFaceInfo.x1;
				rFaceInfo.obj_grin[j].y += rFaceInfo.ynose;
			}
		}
	}

	// reset eye bounce if too many frames without eyes
	if (ebx.update(bFound))
	{
		ebx.reset(EYE_LOST_CT, RUNTIME_INH);
	}
}


void external_action(const bool f)
{
	// TODO: implement
}


int main(int argc, char** argv)
{
	cout << "CPOX CONSOLE" << endl;
	
	String window_name = "Cam";
	String face_cascade_name = "haarcascade_frontalface_alt.xml";
	String eyes_cascade_name = "haarcascade_eye_tree_eyeglasses.xml";
	String grin_cascade_name = "haarcascade_smile.xml";
	//    String grin_cascade_name = "haarcascade_mcs_mouth.xml";

	if (!cc_face.load(haar_cascade_path + face_cascade_name))
	{
		cout << "Face cascade data failed to open." << endl;
		return -1;
	}

	if (!cc_eyes.load(haar_cascade_path + eyes_cascade_name))
	{
		cout << "Eyes cascade data failed to open." << endl;
		return -1;
	}

	if (!cc_grin.load(haar_cascade_path + grin_cascade_name))
	{
		cout << "Grin cascade data failed to open." << endl;
		return -1;
	}

	VideoCapture vcap(0);
	if (!vcap.isOpened())
	{
		cout << "Camera Device failed to open." << endl;
		return -1;
	}

	namedWindow(window_name, WINDOW_AUTOSIZE);

	char key;
	float img_scale = 0.5;
	int prev_inh = STARTUP_INH;
	ostringstream oss;

	// start with warning color
	Scalar sca_box = SCA_YELLOW_MED;

	bool record = false;
	bool go = false;
	int frame_ct = 0;

	FaceInfo face_info;

	// initialize with startup inhibit count
	ebx.reset(EYE_LOST_CT, STARTUP_INH);

	while (1)
	{
		Mat img;
		Mat img_small;
		Mat img_gray;
		int inh = 0;
		int ct = 0;

		// grab image, downsize, convert to gray
		// and run detection
		vcap >> img;
		resize(img, img_small, Size(), img_scale, img_scale);
		cvtColor(img_small, img_gray, COLOR_BGR2GRAY);

		// enable
		if (go)
		{
			// mask will have face and eye boxes
			detect(img_gray, face_info);

			// check inhibit counter
			// non-zero means it was triggered
			// and inhibit period is active
			inh = ebx.getInhibitCt();
			
			if ((prev_inh == 0) && (inh > 0))
			{
				// action (and inhibit) has been triggered
				external_action(true);
				sca_box = SCA_RED_MED;
			}
			else if ((prev_inh > 0) && (inh == 0))
			{
				// action (and inhibit) has completed
				external_action(false);
				sca_box = SCA_BLACK;
			}
			else if (inh > 0)
			{
				// TODO:  external action disable at specific count
			}
			else if (inh == 0)
			{
				// during normal operation (no inhibit)
				// show warning color if eye-miss count is high
				ct = ebx.getMissCount();
				if (ct > EYE_WARN_CT)
				{
					sca_box = SCA_YELLOW_MED;
				}
				else
				{
					sca_box = SCA_BLACK;
				}
			}
			prev_inh = inh;

			// draw face features back into source image
			face_info.rgb_draw_boxes(img_small);
			
			// @TODO -- figure out reliable "smile metric"
			cout << face_info.obj_grin.size() << endl;

			// draw inhibit countdown value in upper left
			// along with status color
			oss << inh;
			rectangle(img_small, Rect(0, 0, 50, 20), sca_box, CV_FILLED);
			rectangle(img_small, Rect(0, 0, 50, 20), SCA_WHITE);
			putText(img_small, oss.str(), Point(10, 14), FONT_HERSHEY_PLAIN, 1.0, SCA_WHITE, 2);
			oss.str("");
		}

		imshow(window_name, img_small);
		
		if (record)
		{
			ostringstream oss;
			oss << "tc" << setw(5) << setfill('0') << frame_ct << ".png";
			//cout << oss.str() << endl;
			imwrite(pathx + oss.str(), img_small);
			frame_ct++;
		}

		// this is where all key input comes from
		// TODO: add tuning keys
		key = waitKey(1);
		if (key == char(27) || key == 'q')
		{
			// esc or q to quit
			break;
		}
		else if (key == 'g')
		{
			// start actively monitoring face
			go = true;
		}
		else if (key == '.')
		{
			record = true;
		}
	}

	// loop was terminated
	// be sure any external action is also halted
	external_action(false);
	return 0;
}
