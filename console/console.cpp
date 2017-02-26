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


typedef std::vector<cv::Rect> tVecRect;


typedef struct tFaceInfo_struct
{
	double nose_ratio;
	double chin_ratio;

	int x1;
	int x2;
	int xhalf;
	int ynose;
	int ychin;
	
	cv::Rect rect_face;
	cv::Rect rect_mouth;
	cv::Rect rect_eye_L;
	cv::Rect rect_eye_R;
	
	std::vector<Rect> obj_grin;

	struct tFaceInfo_struct() :
		nose_ratio(0.5625),	// legacy setting: 5/8
		chin_ratio(1.125)
	{
		// blank
	}

} tFaceInfo;


void detect(const Mat& r, tFaceInfo& rFaceInfo)
{
	std::vector<Rect> obj_face;
	std::vector<Rect> obj_eyeL;
	std::vector<Rect> obj_eyeR;

	const int face_neighbor_ct = 2;
	const int eye_neighbor_ct = 4;

	// demo code does histogram equalization so do the same
	equalizeHist(r, r);

	// assume face will be "big"
	Size size_face(60, 60);

	// eyes will be smaller than face
	Size size_eyes(18, 18);

	// blow away old info
	rFaceInfo.x1 = 0;
	rFaceInfo.x2 = 0;
	rFaceInfo.xhalf = 0;
	rFaceInfo.ynose = 0;
	rFaceInfo.ychin = 0;
	rFaceInfo.rect_face = Rect();
	rFaceInfo.rect_mouth = Rect();
	rFaceInfo.rect_eye_L = Rect();
	rFaceInfo.rect_eye_R = Rect();
	rFaceInfo.obj_grin.clear();

	// assume eyes found
	// this will be applied to debounce
	// if face or eye detections fail (rare)
	bool bFound = false;

	// first find ONE face
	cc_face.detectMultiScale(r, obj_face, 1.1, face_neighbor_ct, 0, size_face);
	if (obj_face.size() == 1)
	{
		const Rect& face = obj_face[0];

		// determine coordinates for face feature regions
		int hnose = static_cast<int>((face.height) * rFaceInfo.nose_ratio);
		int hchin = static_cast<int>((face.height) * rFaceInfo.chin_ratio);
		int x1 = face.x;
		int x2 = face.x + face.width;
		int xhalf = face.x + (face.width / 2);
		int y1 = face.y;
		int ynose = face.y + hnose;
		int ychin = face.y + hchin;

		// y-coord for chin may go out of bounds so adjust if necessary
		if (ychin > r.size().height)
		{
			ychin = r.size().height;
		}

		// copy new face feature info
		rFaceInfo.x1 = x1;
		rFaceInfo.x2 = x2;
		rFaceInfo.rect_face = Rect(Point(x1, y1), Point(x2, ychin));
		rFaceInfo.rect_mouth = Rect(Point(x1, ynose), Point(x2, ychin));
		rFaceInfo.xhalf = xhalf;
		rFaceInfo.ynose = ynose;
		rFaceInfo.ychin = ychin;

		// assign eye search rectangles ("left" is left side of displayed image)
		Rect rect_eyeL = Rect(Point(x1, y1), Point(xhalf, ynose));
		Rect rect_eyeR = Rect(Point(xhalf, y1), Point(x2, ynose));
		Mat eyeL_ROI = r(rect_eyeL);
		Mat eyeR_ROI = r(rect_eyeR);

		// then search for eyes in eye rectangles
		// set found flag even if only one is found
	
		cc_eyes.detectMultiScale(eyeL_ROI, obj_eyeL, 1.1, eye_neighbor_ct, 0, size_eyes);
		if (obj_eyeL.size() >= 1)
		{
			rFaceInfo.rect_eye_L = obj_eyeL[0];
			rFaceInfo.rect_eye_L.x += x1;
			rFaceInfo.rect_eye_L.y += y1;
			bFound = true;
		}

		cc_eyes.detectMultiScale(eyeR_ROI, obj_eyeR, 1.1, eye_neighbor_ct, 0, size_eyes);
		if (obj_eyeR.size() >= 1)
		{
			rFaceInfo.rect_eye_R = obj_eyeR[0];
			rFaceInfo.rect_eye_R.x += xhalf;
			rFaceInfo.rect_eye_R.y += y1;
			bFound = true;
		}

		if (true)
		{
			// try to find grin in mouth area
			// use magic parameter to tune
			// TODO:  tune during start-up?

			int magic = 0;
			Mat grinROI = r(rFaceInfo.rect_mouth);
			int w = (rFaceInfo.rect_mouth.width * 3) / 8;  // min 3/8 of mouth region width
			int h = (rFaceInfo.rect_mouth.height * 1) / 3;  // min 1/3 of mouth region height
			cc_grin.detectMultiScale(grinROI, rFaceInfo.obj_grin, 1.1, magic, 0, Size(w, h));

			// this statement controls whether or not grin
			// is required to determine if face "eye" state is okay
			bFound = bFound && (rFaceInfo.obj_grin.size() > 0);

			// offset the grin boxes
			for (size_t j = 0; j < rFaceInfo.obj_grin.size(); j++)
			{
				rFaceInfo.obj_grin[j].x += x1;
				rFaceInfo.obj_grin[j].y += ynose;
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

	// some convenient color variables
	// start with warning color
	Scalar sblack = Scalar(0, 0, 0);
	Scalar swhite = Scalar(255, 255, 255);
	Scalar syellow = Scalar(0, 192, 192);
	Scalar sred = Scalar(0, 0, 192);
	Scalar sgreen = Scalar(0, 255, 0);
	Scalar sbox = syellow;

	bool record = false;
	bool go = false;
	int frame_ct = 0;

	tFaceInfo face_info;

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
				sbox = sred;
			}
			else if ((prev_inh > 0) && (inh == 0))
			{
				// action (and inhibit) has completed
				external_action(false);
				sbox = sblack;
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
					sbox = syellow;
				}
				else
				{
					sbox = sblack;
				}
			}
			prev_inh = inh;

			// draw face features
			rectangle(img_small, face_info.rect_mouth, syellow);
			rectangle(img_small, face_info.rect_face, swhite);
			rectangle(img_small, face_info.rect_eye_L, sgreen);
			rectangle(img_small, face_info.rect_eye_R, sgreen);
			for (size_t j = 0; j < face_info.obj_grin.size(); j++)
			{
				rectangle(img_small, face_info.obj_grin[j], sred);
			}
			
			// @TODO -- figure out reliable "smile metric"
			cout << face_info.obj_grin.size() << endl;

			// draw inhibit countdown value in upper left
			// along with status color
			oss << inh;
			rectangle(img_small, Rect(0, 0, 50, 20), sbox, CV_FILLED);
			rectangle(img_small, Rect(0, 0, 50, 20), swhite);
			putText(img_small, oss.str(), Point(10, 14), FONT_HERSHEY_PLAIN, 1.0, swhite, 2);
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
