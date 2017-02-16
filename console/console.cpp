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


void detect(const Mat& r, Mat& rmask)
{
	std::vector<Rect> obj_face;
	std::vector<Rect> obj_eyes;
	std::vector<Rect> obj_grin;

	const int face_neighbor_ct = 2;
	const int eye_neighbor_ct = 4;

	// demo code does histogram equalization so do the same
	equalizeHist(r, r);

	// assume face will be "big"
	Size size_face(60, 60);

	// eyes will be smaller than face
	Size size_eyes(18, 18);

	// assume eyes found
	// this will be applied to debounce
	// if face or eye detections fail (rare)
	bool bFound = false;

	// first find one face
	cc_face.detectMultiScale(r, obj_face, 1.1, face_neighbor_ct, 0, size_face);
	if (obj_face.size() == 1)
	{
		for (size_t i = 0; i < obj_face.size(); i++)
		{
			// draw face box with eye and mouth divider lines
			// vertical line between eyes
			// horizontal line 5/8 from top of face box
			// to separate mouth region and rest of face
			int yfrac = (obj_face[i].height * 5) / 8;
			int halfx = obj_face[i].x + obj_face[i].width / 2;
			int y1 = obj_face[i].y;
			int y2 = obj_face[i].y + yfrac;
			int x1 = obj_face[i].x;
			int x2 = obj_face[i].x + obj_face[i].width;
			rectangle(rmask, obj_face[i], 255);
			line(rmask, Point(halfx, y1), Point(halfx, y2), 255);
			line(rmask, Point(x1, y2), Point(x2, y2), 255);

			// seek eyes in face region
			// use "4" for rectangle threshold (fewer false detections)
			// assume frame is "good" if more than 2 eyes
			
			Rect rectz = obj_face[i];
			rectz.height = yfrac;
			Mat faceROI = r(rectz);
			cc_eyes.detectMultiScale(faceROI, obj_eyes, 1.1, eye_neighbor_ct, 0, size_eyes);
			bFound = (obj_eyes.size() > 0);
			if (obj_eyes.size() > 2)
			{
				// just FYI
				//cout << "invalid eye count" << endl;
				continue;
			}

			// draw eyes
			for (size_t j = 0; j < obj_eyes.size(); j++)
			{
				int x = obj_face[i].x + obj_eyes[j].x;
				int y = obj_face[i].y + obj_eyes[j].y;
				int w = obj_eyes[i].width;
				int h = obj_eyes[i].height;
				Rect rectEye(x, y, w, h);
				rectangle(rmask, rectEye, 255);
			}

			// try to find grin in mouth area
			// use magic parameter to tune
			// TODO:  tune during start-up?
			
			int magic = 0;
			Rect rectg = obj_face[i];
			rectg.height -= yfrac;
			rectg.y += yfrac;
			Mat grinROI = r(rectg);
			int w = (rectg.width * 3) / 8;  // min 3/8 of mouth region width
			int h = (rectg.height * 1) / 3;  // min 1/3 of mouth region height
			cc_grin.detectMultiScale(grinROI, obj_grin, 1.1, magic, 0, Size(w, h));

			// this statement controls whether or not grin
			// is required to determine if face "eye" state is okay
			if (true)
			{
				bFound = bFound && (obj_grin.size() > 0);
			}

			// draw grin
			for (size_t j = 0; j < obj_grin.size(); j++)
			{
				int x = obj_face[i].x + obj_grin[j].x;
				int y = obj_face[i].y + obj_grin[j].y + yfrac;
				int w = obj_grin[i].width;
				int h = obj_grin[i].height;
				Rect rectGrin(x, y, w, h);
				rectangle(rmask, rectGrin, 255);
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
	Scalar sbox = syellow;

	bool record = false;
	bool go = false;
	int frame_ct = 0;

	// initialize with startup inhibit count
	ebx.reset(EYE_LOST_CT, STARTUP_INH);

	while (1)
	{
		Mat img;
		Mat img_small;
		Mat img_gray;
		Mat img_mask;
		Mat img_final;
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
			img_mask = Mat::zeros(img_gray.size(), img_gray.type());
			detect(img_gray, img_mask);

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

			// merge gray images to create RGB image with green face and eye boxes
			// face is blurred to protect identity
			vector<Mat> channels;
			split(img_small, channels);
			channels[1] = channels[1] | img_mask;
			merge(channels, img_final);

			// draw inhibit countdown value in upper left
			// along with status color
			oss << inh;
			rectangle(img_final, Rect(0, 0, 50, 20), sbox, CV_FILLED);
			rectangle(img_final, Rect(0, 0, 50, 20), swhite);
			putText(img_final, oss.str(), Point(10, 14), FONT_HERSHEY_PLAIN, 1.0, swhite, 2);
			oss.str("");
		}
		else
		{
			// not active
			// just show video
			img_final = img_small;
		}

		imshow(window_name, img_final);
		if (record)
		{
			ostringstream oss;
			oss << "tc" << setw(5) << setfill('0') << frame_ct << ".png";
			//cout << oss.str() << endl;
			imwrite(pathx + oss.str(), img_final);
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
