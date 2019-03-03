
#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "CVMain.h"

using namespace cv;

CVMain::CVMain()
{
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

    if (!cc_face.load(face_cascade_name))
    {
        std::cout << "Face cascade data failed to open!" << std::endl;;
        result = false;
    }

    if (!cc_eyes.load(eyes_cascade_name))
    {
        std::cout << "Eyes cascade data failed to open!" << std::endl;;
        result = false;
    }

    if (!cc_grin.load(grin_cascade_name))
    {
        std::cout << "Grin cascade data failed to open!" << std::endl;;
        result = false;
    }

    return result;
}

bool CVMain::detect(cv::Mat& r, FaceInfo& rFaceInfo)
{
    std::vector<Rect> obj_face;
    std::vector<Rect> obj_eyeL;
    std::vector<Rect> obj_eyeR;

    // tuning parameters
    const double face_scale_factor = 1.1;
    const double eyes_scale_factor = 1.1;
    const double grin_scale_factor = 1.1;
    const double min_face_scale = 0.20;
    const double min_eyes_scale = 0.25;
    const int face_neighbor_ct = 2;
    const int eye_neighbor_ct = 1;

    // demo code does histogram equalization so do the same
    equalizeHist(r, r);

    // minimum face width is some fraction of input image width
    Size min_size_face(
        static_cast<int>(r.size().width * min_face_scale),
        static_cast<int>(r.size().width * min_face_scale));

    // blow away old info
    rFaceInfo.reset_results();

    // assume nothing will be found
    bool bFound = false;

    // first find ONE face
    cc_face.detectMultiScale(r, obj_face, face_scale_factor, face_neighbor_ct, 0, min_size_face);
    if (obj_face.size() == 1)
    {
        // found a face so apply its dimensions
        rFaceInfo.apply_face(r.size(), obj_face[0]);
        bFound = true;

        if (rFaceInfo.is_eyes_detect_enabled)
        {
            // search for eyes in eye regions-of-interest
            // set found flag even if only one eye is found

            bFound = false;
            Mat eyeL_ROI = r(rFaceInfo.rect_eyeL_roi);
            Mat eyeR_ROI = r(rFaceInfo.rect_eyeR_roi);

            // minimum eye width is some fraction of eye ROI width
            // both eyes ROI are the same size
            
            Size min_size_eyes(
                static_cast<int>(eyeL_ROI.size().width * min_eyes_scale),
                static_cast<int>(eyeL_ROI.size().width * min_eyes_scale));

            int hcta = 20;
            int hctb = 10;

            cc_eyes.detectMultiScale(eyeL_ROI, obj_eyeL, eyes_scale_factor, eye_neighbor_ct, 0, min_size_eyes);
            if (obj_eyeL.size() >= 1)
            {
                rFaceInfo.apply_eyeL(obj_eyeL[0]);

                // look for iris circle
                Mat irisL = eyeL_ROI(rFaceInfo.rect_irisL_roi);
                rFaceInfo.get_circlesL().clear();
                HoughCircles(irisL, rFaceInfo.get_circlesL(), HOUGH_GRADIENT, 1, eyeL_ROI.rows / 16, hcta, hctb, eyeL_ROI.rows / 16, eyeL_ROI.rows / 8);

                bFound = true;
            }

            cc_eyes.detectMultiScale(eyeR_ROI, obj_eyeR, eyes_scale_factor, eye_neighbor_ct, 0, min_size_eyes);
            if (obj_eyeR.size() >= 1)
            {
                rFaceInfo.apply_eyeR(obj_eyeR[0]);

                // look for iris circle
                Mat irisR = eyeR_ROI(rFaceInfo.rect_irisR_roi);
                rFaceInfo.get_circlesR().clear();
                HoughCircles(irisR, rFaceInfo.get_circlesR(), HOUGH_GRADIENT, 1, eyeR_ROI.rows / 16, hcta, hctb, eyeR_ROI.rows / 16, eyeR_ROI.rows / 8);

                bFound = true;
            }
        }

        if (rFaceInfo.is_grin_detect_enabled)
        {
            // try to find grin in mouth area
            // use magic parameter to tune

            int magic = (rFaceInfo.smile_thr * 5) + 1;

            Mat grinROI = r(rFaceInfo.rect_grin_roi);

#if 0
            // try fixed-size 256 pixel wide box for smile detection
            Mat grinROI;
            double rat = 256.0 / grinROI_0.size().width;
            resize(grinROI_0, grinROI, Size(), rat, rat);
#endif
            
            int wmin = (grinROI.size().width * 3) / 8;
            int hmin = (grinROI.size().height * 3) / 8;
            int wmax = (grinROI.size().width * 3) / 4;
            int hmax = (grinROI.size().height * 3) / 4;
            cc_grin.detectMultiScale(grinROI, rFaceInfo.obj_grin, grin_scale_factor, magic, 0, Size(wmin, hmin), Size(wmax, hmax));

            // this statement controls whether or not grin
            // is required to determine if face "eye" state is okay
            bFound = bFound && (rFaceInfo.obj_grin.size() > 0);

            // offset the grin boxes
            for (size_t j = 0; j < rFaceInfo.obj_grin.size(); j++)
            {
                rFaceInfo.obj_grin[j].x += rFaceInfo.xa;
                rFaceInfo.obj_grin[j].y += rFaceInfo.ygrin;
            }
        }
    }

    return bFound;
}

void CVMain::make_movie(const double fps, const std::string& rspath, const std::list<std::string>& rListOfPNG)
{
    // determine size of frames
    const std::string& rs = rListOfPNG.front();
    Mat img = imread(rs);
    Size img_sz = img.size();

    std::string sname = rspath + "\\movie.wmv";

    // build movie from separate frames
    // TODO -- may need to change FPS on different systems
    VideoWriter vw = VideoWriter(sname,
        VideoWriter::fourcc('W', 'M', 'V', '2'),
        fps, img_sz);

    if (vw.isOpened())
    {
        for (const auto& r : rListOfPNG)
        {
            Mat img = imread(r);
            vw.write(img);
        }
    }
}
