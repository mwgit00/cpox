
#include <iostream>
#include "CVMain.h"

using namespace cv;

CVMain::CVMain()
{
    // assume face will be "big"
    // and eyes will be smaller
    // suitable limits here will increase frame rate
    // TODO-- this might depend on scale(currently fixed at 0.5)
//    size_face = (60, 60);
    //size_eyes = (18, 18);

    // grin detection tweak
    // - use 2 for mouth detector
    // - use big number like 70-140 for smile detector
    // TODO -- tune during start - up ?
//    magic = 140;
}

CVMain::~CVMain()
{
}

bool CVMain::load_cascades(const std::string& path)
{
    bool result = true;
    
    // try to load standard OpenCV face/eyes/grin cascades
    std::string face_cascade_name = path + "haarcascade_frontalface_alt.xml";
    std::string eyes_cascade_name = path + "haarcascade_eye_tree_eyeglasses.xml";
    std::string grin_cascade_name = path + "haarcascade_smile.xml";
    //std::string grin_cascade_name = "haarcascade_mcs_mouth.xml"

    if (!cc_face.load(face_cascade_name))
    {
        std::cout << "Face cascade data failed to open." << std::endl;;
        result = false;
    }

    if (!cc_eyes.load(eyes_cascade_name))
    {
        std::cout << "Eyes cascade data failed to open." << std::endl;;
        result = false;
    }

    if (!cc_grin.load(grin_cascade_name))
    {
        std::cout << "Grin cascade data failed to open." << std::endl;;
        result = false;
    }

    return result;
}

bool CVMain::detect(cv::Mat& r, FaceInfo& rFaceInfo)
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
