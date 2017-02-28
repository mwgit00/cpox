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

#include <ctime>
#include <chrono>

#include "defs.h"
#include "FaceInfo.h"
#include "MissedDetectionTrigger.h"

using namespace std;
using namespace cv;


CascadeClassifier cc_face;
CascadeClassifier cc_eyes;
CascadeClassifier cc_grin;

const double EYE_MISS_CT_MS = 3000.0;
const double EYE_WARN_CT_MS = 2000.0;
const double RUNTIME_INH_MS = 5000.0;
const double STARTUP_INH_MS = 8000.0;


// global detection and inhibit timer object
MissedDetectionTrigger ebx;

String haar_cascade_path = "C:\\opencv-3.2.0\\opencv\\build\\etc\\haarcascades\\";
String path = "~/work/cpox/";
String pathx = "~/work/cpox/movie/";


bool detect(const Mat& r, FaceInfo& rFaceInfo)
{
    std::vector<Rect> obj_face;
    std::vector<Rect> obj_eyeL;
    std::vector<Rect> obj_eyeR;

    const double face_scale_factor = 1.1;
    const double eyes_scale_factor = 1.05;
    const double grin_scale_factor = 1.1;
    const int face_neighbor_ct = 2;
    const int eye_neighbor_ct = 3;

    // demo code does histogram equalization so do the same
    equalizeHist(r, r);

    // assume face will be "big"
    Size min_size_face(60, 60);

    // eyes will be smaller than face
    Size min_size_eyes(8, 8);

    // blow away old info
    rFaceInfo.reset_results();

    // assume nothing will be found
    bool bFound = false;

    // first find ONE face
    cc_face.detectMultiScale(r, obj_face, face_scale_factor, face_neighbor_ct, 0, min_size_face);
    if (obj_face.size() == 1)
    {
        // found face
        rFaceInfo.apply_face(r.size(), obj_face[0]);
        bFound = true;

        if (rFaceInfo.is_eyes_enabled)
        {
            // then search for eyes in eye rectangles
            // set found flag even if only one is found

            bFound = false;
            Mat eyeL_ROI = r(rFaceInfo.rect_eyeL_roi);
            Mat eyeR_ROI = r(rFaceInfo.rect_eyeR_roi);

            cc_eyes.detectMultiScale(eyeL_ROI, obj_eyeL, eyes_scale_factor, eye_neighbor_ct, 0, min_size_eyes);
            if (obj_eyeL.size() >= 1)
            {
                rFaceInfo.apply_eyeL(obj_eyeL[0]);
                bFound = true;
            }

            cc_eyes.detectMultiScale(eyeR_ROI, obj_eyeR, eyes_scale_factor, eye_neighbor_ct, 0, min_size_eyes);
            if (obj_eyeR.size() >= 1)
            {
                rFaceInfo.apply_eyeR(obj_eyeR[0]);
                bFound = true;
            }
        }

        if (rFaceInfo.is_grin_enabled)
        {
            // try to find grin in mouth area
            // use magic parameter to tune
            // TODO:  tune during start-up?

            int magic = 0;
            
            Mat grinROI = r(rFaceInfo.rect_grin_roi);
            int w = (grinROI.size().width * 3) / 8;  // min 3/8 of mouth region width
            int h = (grinROI.size().height * 1) / 3;  // min 1/3 of mouth region height
            cc_grin.detectMultiScale(grinROI, rFaceInfo.obj_grin, grin_scale_factor, magic, 0, Size(w, h));

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

    return bFound;
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
    ostringstream oss;

    // start with "grayed out" countdown box
    Scalar sca_box = SCA_GRAY;

    bool record = false;
    bool go = false;
    int frame_ct = 0;

    FaceInfo face_info;

    // frame rate time points
    std::chrono::time_point<std::chrono::steady_clock> t_prev =
        std::chrono::high_resolution_clock::now();
    std::chrono::time_point<std::chrono::steady_clock> t_curr;

    while (1)
    {
        Mat img;
        Mat img_small;
        Mat img_gray;
        int inh_seconds = 0;
        int ct = 0;

        // grab image, downsize, convert to gray
        // and run detection
        vcap >> img;
        resize(img, img_small, Size(), img_scale, img_scale);
        cvtColor(img_small, img_gray, COLOR_BGR2GRAY);

        // run the detection with the current options
        bool bFound = detect(img_gray, face_info);

        // enable
        if (go)
        {
            ebx.update(bFound);

            // handle the one-shot events...
            if (ebx.isTriggered())
            {
                // loss-of-detection has been triggered
                external_action(true);
                sca_box = SCA_RED_MED;
            }
            else if (ebx.isDetecting())
            {
                // inhibit has completed
                // use runtime timing parameters
                ebx.set(EYE_MISS_CT_MS, RUNTIME_INH_MS);
                external_action(false);
                sca_box = SCA_BLACK;
            }

            inh_seconds = ebx.getInhibitSecondsRemaining();

            if (inh_seconds > 0)
            {
                // TODO:  external action disable at specific count
            }
            else
            {
                // during normal operation (no inhibit)
                // show warning color if eye-miss count is high
                if (ebx.getDetectMsecRemaining() < EYE_WARN_CT_MS)
                {
                    sca_box = SCA_YELLOW_MED;
                }
                else
                {
                    sca_box = SCA_BLACK;
                }
            }

        }

        // @TODO -- figure out reliable "smile metric"
        cout << face_info.obj_grin.size() << endl;

        // draw inhibit countdown value in upper left with status color
        oss << inh_seconds;
        rectangle(img_small, Rect(0, 0, 50, 20), sca_box, CV_FILLED);
        rectangle(img_small, Rect(0, 0, 50, 20), SCA_WHITE);
        putText(img_small, oss.str(), Point(10, 14), FONT_HERSHEY_PLAIN, 1.0, SCA_WHITE, 2);
        oss.str("");

        // draw face features back into source image
        face_info.rgb_draw_boxes(img_small);

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
            // use startup timing parameters
            sca_box = SCA_YELLOW_MED;
            ebx.set(EYE_MISS_CT_MS, STARTUP_INH_MS);
            ebx.start();
            go = true;
        }
        else if (key == 'h')
        {
            // stop actively monitoring face
            external_action(false);
            sca_box = SCA_GRAY;
            inh_seconds = 0;
            ebx.reset();
            go = false;
        }
        else if (key == ':')
        {
            // toggle eyes detection
            face_info.is_eyes_enabled = !face_info.is_eyes_enabled;
        }
        else if (key == ')')
        {
            // toggle grin detection
            face_info.is_grin_enabled = !face_info.is_grin_enabled;
        }
        else if (key == '.')
        {
            record = true;
        }

#if 0
        t_curr = std::chrono::high_resolution_clock::now();
        double x = std::chrono::duration<double>(t_curr - t_prev).count();
        int fr = static_cast<int>((1.0 / x) + 0.5);
        cout << fr << endl;
        t_prev = t_curr;
#endif
    }

    // loop was terminated
    // be sure any external action is also halted
    external_action(false);
    return 0;
}