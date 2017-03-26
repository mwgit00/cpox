#include "Windows.h"
#include "resource.h"

#include <iostream>
#include <iomanip>
#include <sstream>

#include "opencv2/highgui/highgui.hpp"

#include "util.h"
#include "TTSTask.h"
#include "COMTask.h"
#include "AppMain.h"

/*

AppMain class
- Worker process startup
- Main loop with Face/Eye detection
- Optional Listen-and-Repeat speech recognition mode
- Camera and diagnostic view
- User control via keyboard

*/


using namespace cv;


AppMain::AppMain() :
    is_looping(true),
    is_eyes_detect_enabled(true),
    is_grin_detect_enabled(false),
    is_tts_up(false),
    is_com_up(false),
    s_strikes(""),
    s_current_phrase(""),
    n_z(0),
    is_com_blinky_on(false),
    s_ack_level(S_COM_NO_ACK),
    zoom_ct(0), // 1x
    pan_ct(0),  // centered
    tilt_ct(0)  // centered
{
    // these keys are converted to events for state machines
    cvsm_keys = { KEY_GO, KEY_HALT, KEY_LISTEN };

    ui_func_map[KEY_ESC] = &AppMain::UIBreak;
    ui_func_map[KEY_QUIT] = &AppMain::UIBreak;
    ui_func_map[KEY_EYES] = &AppMain::UIEyes;
    ui_func_map[KEY_GRIN] = &AppMain::UIGrin;
    ui_func_map[KEY_HELP] = &AppMain::UIHelp;
    ui_func_map[KEY_EXTON] = &AppMain::UITestExt;
    ui_func_map[KEY_VIDREC] = &AppMain::UIRecord;
    ui_func_map[KEY_SAY] = &AppMain::UITestSay;
    ui_func_map[KEY_TTSREC] = &AppMain::UITestSpeechRec;
    ui_func_map[KEY_NEWMOV] = &AppMain::UIMakeMovie;
    ui_func_map[KEY_ZOOMGT] = &AppMain::UIZoomIn;
    ui_func_map[KEY_ZOOMLT] = &AppMain::UIZoomOut;
    ui_func_map[KEY_PANL] = &AppMain::UIPanL;
    ui_func_map[KEY_PANR] = &AppMain::UIPanR;
    ui_func_map[KEY_TILTU] = &AppMain::UITiltU;
    ui_func_map[KEY_TILTD] = &AppMain::UITiltD;
    ui_func_map[KEY_ZOOM0] = &AppMain::UIResetZoom;
    ui_func_map[KEY_PT0] = &AppMain::UIResetPanTilt;
    ui_func_map[KEY_TEST1] = &AppMain::UITest1;
    ui_func_map[KEY_TEST2] = &AppMain::UITest2;
    ui_func_map[KEY_TEST3] = &AppMain::UITest3;
    ui_func_map[KEY_TEST4] = &AppMain::UITest4;

    action_func_map[FSMEventCode::E_TTS_SAY] = &AppMain::ActionTTSSay;
    action_func_map[FSMEventCode::E_TTS_UP] = &AppMain::ActionTTSUp;
    action_func_map[FSMEventCode::E_SR_PHRASE] = &AppMain::ActionSRPhrase;
    action_func_map[FSMEventCode::E_SR_REC] = &AppMain::ActionSRRec;
    action_func_map[FSMEventCode::E_SR_STRIKES] = &AppMain::ActionSRStrikes;
    action_func_map[FSMEventCode::E_COM_XON] = &AppMain::ActionComXON;
    action_func_map[FSMEventCode::E_COM_XOFF] = &AppMain::ActionComXOFF;
    action_func_map[FSMEventCode::E_COM_LEVEL] = &AppMain::ActionComLevel;
    action_func_map[FSMEventCode::E_COM_UP] = &AppMain::ActionComUp;
    action_func_map[FSMEventCode::E_COM_ACK] = &AppMain::ActionComAck;

    // info for frame capture and recording

    record_sfps = "???";
    record_path = "c:\\work\\movie\\";
    is_record_path_ok = util::IsPathOK(record_path);
    is_record_enabled = false;
    record_frame_ct = 0;
    record_clip = 0;
    record_fps_ct = 0;
}

AppMain::~AppMain()
{
}

void AppMain::check_z()
{
    // timer for output "Z" test
    if (n_z > 0)
    {
        n_z -= 1;
        if (n_z == 0)
        {
            external_action(false);
        }
    }
}

void AppMain::reset_fps()
{
    // clear FPS calculation data
    t_prev = std::chrono::high_resolution_clock::now();
    record_fps_ct = 0;
    record_sfps = "???";
}

void AppMain::update_fps()
{
    // recalculate frames-per-second after this many frames
    const int FPS_FRAME_CT = 20;
    record_fps_ct += 1;
    if (record_fps_ct == FPS_FRAME_CT)
    {
        // calculate elapsed time for 100 frames
        std::chrono::time_point<std::chrono::steady_clock> t_curr =
            std::chrono::high_resolution_clock::now();
        double tx = std::chrono::duration<double>(t_curr - t_prev).count();

        // convert to frame rate string
        double fps = static_cast<double>(record_fps_ct) / tx;
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(1) << fps;
        record_sfps = oss.str();

        // reset for next measurement
        t_prev = t_curr;
        record_fps_ct = 0;
    }
}

void AppMain::update_com_status()
{
    std::chrono::time_point<std::chrono::steady_clock> t_curr =
        std::chrono::high_resolution_clock::now();
    double tx = std::chrono::duration<double>(t_curr - t_last_ack).count();

    // no COM activity in 2 seconds so turn off blinky
    // and black the ack level string
    if (tx > 2.0)
    {
        is_com_blinky_on = false;
        s_ack_level = S_COM_NO_ACK;
    }
}

void AppMain::record_frame(cv::Mat& frame)
{
    // record frames to sequentially numbered files if enabled
    // file names will also contain a two-digit clip ID
    if (is_record_path_ok && is_record_enabled)
    {
        std::ostringstream oss;
        if (record_path.back() != '\\')
        {
            // add slash if path doesn't end with one
            oss << "\\";
        }
        oss << "img";
        oss << "_" << std::setfill('0') << std::setw(2) << record_clip;
        oss << "_" << std::setfill('0') << std::setw(5) << record_frame_ct;
        oss << ".png";
        
        std::string file_path = record_path + oss.str();
        record_frame_ct += 1;
        imwrite(file_path, frame);
    }
}

void AppMain::external_action(const bool flag, const uint32_t data)
{
    if (is_com_up)
    {
        // sends command to an external serial device.
        if (flag)
        {
            com_events.push(FSMEvent(FSMEventCode::E_COM_XON, data));
            std::cout << util::GetString(IDS_APP_EXT_ON) << " " << data << std::endl;
        }
        else
        {
            com_events.push(FSMEvent(FSMEventCode::E_COM_XOFF));
            std::cout << util::GetString(IDS_APP_EXT_OFF) << std::endl;
        }
    }
}

void AppMain::show_help()
{
    // press '?' while monitor has focus
    // in order to see this menu
    std::cout << util::GetKeyHelpString(KEY_QUIT, IDS_HELP_KEY_QUIT) << std::endl;
    std::cout << util::GetKeyHelpString(KEY_HELP, IDS_HELP_KEY_HELP) << std::endl;
    std::cout << util::GetKeyHelpString(KEY_EYES, IDS_HELP_KEY_EYES) << std::endl;
    std::cout << util::GetKeyHelpString(KEY_GRIN, IDS_HELP_KEY_GRIN) << std::endl;
    std::cout << util::GetKeyHelpString(KEY_GO, IDS_HELP_KEY_GO) << std::endl;
    std::cout << util::GetKeyHelpString(KEY_HALT, IDS_HELP_KEY_HALT) << std::endl;
    std::cout << util::GetKeyHelpString(KEY_LISTEN, IDS_HELP_KEY_LISTEN) << std::endl;
    std::cout << util::GetKeyHelpString(KEY_VIDREC, IDS_HELP_KEY_VIDREC) << std::endl;
    std::cout << util::GetKeyHelpString(KEY_NEWMOV, IDS_HELP_KEY_NEWMOV) << std::endl;
    std::cout << util::GetKeyHelpString(KEY_ZOOMGT, IDS_HELP_KEY_ZOOMGT) << std::endl;
    std::cout << util::GetKeyHelpString(KEY_ZOOMLT, IDS_HELP_KEY_ZOOMLT) << std::endl;
    std::cout << util::GetKeyHelpString(KEY_ZOOM0, IDS_HELP_KEY_ZOOM0) << std::endl;
    std::cout << util::GetKeyHelpString(KEY_PANL, IDS_HELP_KEY_PANL) << std::endl;
    std::cout << util::GetKeyHelpString(KEY_PANR, IDS_HELP_KEY_PANR) << std::endl;
    std::cout << util::GetKeyHelpString(KEY_TILTU, IDS_HELP_KEY_TILTU) << std::endl;
    std::cout << util::GetKeyHelpString(KEY_TILTD, IDS_HELP_KEY_TILTD) << std::endl;
    std::cout << util::GetKeyHelpString(KEY_PT0, IDS_HELP_KEY_PT0) << std::endl;
    std::cout << util::GetKeyHelpString(KEY_SAY, IDS_HELP_KEY_SAY) << std::endl;
    std::cout << util::GetKeyHelpString(KEY_TTSREC, IDS_HELP_KEY_TTSREC) << std::endl;
    std::cout << util::GetKeyHelpString(KEY_EXTON, IDS_HELP_KEY_EXTON) << std::endl;
    std::cout << "ESC - " << util::GetString(IDS_HELP_KEY_QUIT) << "." << std::endl;
}

void AppMain::show_monitor_window(cv::Mat& img, FaceInfo& rFI, const std::string& rsfps)
{
    // update display items
    Scalar status_color = cvsm.Snapshot().color;
    std::string s_label = cvsm.Snapshot().label;

    // draw face boxes
    Mat img_final = img;
    rFI.rgb_draw_boxes(img_final);
    // FIXME std::cout << rFI.obj_grin.size() << std::endl;

    int wn = 54;  // width of status boxes
    int hn = 20;  // height of status boxes

    // draw status label in upper left
    // along with status color
    rectangle(img_final, Rect(0, 0, wn, hn), status_color, CV_FILLED);
    rectangle(img_final, Rect(0, 0, wn, hn), SCA_WHITE);
    putText(img_final, s_label, Point(10, 14), FONT_HERSHEY_PLAIN, 1.0,
        SCA_WHITE, 2);

    // mode state icon box
    rectangle(img_final, Rect(Point(0, hn), Point(wn, hn * 2)), SCA_PURPLE, CV_FILLED);
    rectangle(img_final, Rect(Point(0, hn), Point(wn, hn * 2)), SCA_WHITE);

    // strike count display
    Scalar speech_mode_color = psm.Snapshot().color;
    rectangle(img_final, Rect(Point(0, hn * 2), Point(wn, hn * 3)),
        speech_mode_color, CV_FILLED);
    rectangle(img_final, Rect(Point(0, hn * 2), Point(wn, hn * 3)),
        SCA_WHITE);
    putText(img_final, s_strikes, Point(10, hn * 2 + 14),
        FONT_HERSHEY_PLAIN, 1.0, SCA_WHITE, 2);

    // frames per second and recording status
    Scalar fps_color = (is_record_enabled) ? SCA_RED : SCA_BLACK;
    rectangle(img_final, Rect(Point(0, hn * 3), Point(wn, hn * 4)),
        fps_color, CV_FILLED);
    rectangle(img_final, Rect(Point(0, hn * 3), Point(wn, hn * 4)),
        SCA_WHITE);
    putText(img_final, rsfps, Point(10, (hn * 3) + 14),
        FONT_HERSHEY_PLAIN, 1.0, SCA_WHITE, 2);

    // COM status
    Scalar com_color = (is_com_up) ? SCA_PURPLE : SCA_GRAY;
    rectangle(img_final, Rect(Point(0, hn * 4), Point(wn, hn * 5)),
        com_color, CV_FILLED);
    rectangle(img_final, Rect(Point(0, hn * 4), Point(wn, hn * 5)),
        SCA_WHITE);
    rectangle(img_final, Rect(Point(4, (hn * 4) + 4), Point(4 + 12, (hn * 5) - 4)),
        is_com_blinky_on ? SCA_BLUE_GREEN : SCA_BLACK, CV_FILLED);
    putText(img_final, s_ack_level, Point(24, hn * 4 + 14),
        FONT_HERSHEY_PLAIN, 1.0, SCA_WHITE, 2);

    // draw speech recognition progress (timeout) bar if active
    // just a black rectangle that gets filled with gray blocks
    // there's a yellow warning bar at ideal timeout time
    int x = psm.Snapshot().prog;
    if (x > 0)
    {
        int rec_sec = FSMPhrase::REC_TIMEOUT_SEC;
        int wb = 10;
        int x1 = wn;
        int x2 = wn + (rec_sec - x) * wb;
        int x3 = wn + rec_sec * wb;
        int xtrg = x1 + 12 * wb;
        rectangle(img_final, Rect(Point(x1, 0), Point(x2, hn)), SCA_GRAY, CV_FILLED);
        rectangle(img_final, Rect(Point(x2, 0), Point(x3, hn)), SCA_BLACK, CV_FILLED);
        line(img_final, Point(xtrg, 0), Point(xtrg, hn - 1), SCA_YELLOW);
        rectangle(img_final, Rect(Point(x1, 0), Point(x3, hn)), SCA_WHITE);
    }

    // draw eye detection state indicator (pair of eyes)
    Scalar sca_eyes = (is_eyes_detect_enabled) ? SCA_WHITE : SCA_GRAY;
    {
        int e_y = 28;
        int e_x = 8;
        int e_dx = 8;
        circle(img_final, Point(e_x, e_y), 3, sca_eyes, CV_FILLED);
        circle(img_final, Point(e_x + e_dx, e_y), 3, sca_eyes, CV_FILLED);
        if (is_eyes_detect_enabled)
        {
            circle(img_final, Point(e_x, e_y), 1, SCA_BLACK, CV_FILLED);
            circle(img_final, Point(e_x + e_dx, e_y), 1, SCA_BLACK, CV_FILLED);
        }
    }

    // draw grin detection state indicator (curve like a grin)
    Scalar sca_grin = (is_grin_detect_enabled) ? SCA_WHITE : SCA_GRAY;
    {
        int g_x = 34;
        int g_y = 28;
        ellipse(img_final, Point(g_x, g_y), Point(5, 3), 0, 0, 180,
            sca_grin, 2);
    }

    // record frame if enabled and update monitor
    record_frame(img_final);
    imshow("CPOX Monitor", img_final);
}

void AppMain::wait_and_check_keys(void)
{
    char key = waitKey(1);

    if (key >= 0)
    {
        if (ui_func_map.count(key))
        {
            // handle UI functions
            tVVFuncPtr p = ui_func_map[key];
            (this->*p)();
        }
        else if (cvsm_keys.count(key))
        {
            // a key press that affects a state machine
            // will be stuffed in an event
            app_events.push(FSMEvent(FSMEventCode::E_KEY, key));
        }
    }
}

void AppMain::loop()
{
    // main application loop
    // - Do frame acquisition
    // - Collect events
    // - Apply events to state machine
    // - Update display
    // - Check keyboard input

    // need a 0 as argument
    VideoCapture vcap(0);
    if (!vcap.isOpened())
    {
        std::cout << util::GetString(IDS_APP_CAMERA_FAIL) << std::endl;
        ///////
        return;
        ///////
    }

    // grab an image to determine its size
    Mat img_sample;
    vcap >> img_sample;
    Size capture_size = img_sample.size();

    // determine size of image for console viewer
    // scale may need to change depending on camera
    double img_scale = 0.5;
    Size viewer_size = Size(
        static_cast<int>(capture_size.width * img_scale),
        static_cast<int>(capture_size.height * img_scale));

    // determine parameters for 1x to 4x zoom
    const int zoom_fac = 4;
    const int wzoom = capture_size.width;
    const int hzoom = capture_size.height;
    const int max_zoom_offset_w = (wzoom / 2) - ((wzoom / zoom_fac) / 2);
    const int max_zoom_offset_h = (hzoom / 2) - ((hzoom / zoom_fac) / 2);
    const int zoom_step_w = (max_zoom_offset_w) / ZOOM_STEPS;
    const int zoom_step_h = (max_zoom_offset_h) / ZOOM_STEPS;

    reset_fps();

    while (is_looping)
    {
        Mat img;
        Mat img_viewer;
        Mat img_gray;
        FaceInfo face_info;

        // process images frame-by-frame
        // grab image, resize, extract ROI, run detection
        // b_found will be result of face/eye/grin detection
        // FaceInfo has data for drawing rectangles for what was detected
        vcap >> img;

        // calculate ROI for zoom
        int zoom_offset_w = zoom_step_w * zoom_ct;
        int zoom_offset_h = zoom_step_h * zoom_ct;
        Rect zoom_rect(
            zoom_offset_w + (pan_ct * zoom_step_w),
            zoom_offset_h + (tilt_ct * zoom_step_h),
            capture_size.width - 2 * zoom_offset_w,
            capture_size.height - 2 * zoom_offset_h);

        // rescale zoom image to fit viewer
        Mat img_zoom = img(zoom_rect);
        resize(img_zoom, img_viewer, viewer_size);
        cvtColor(img_viewer, img_gray, COLOR_BGR2GRAY);

        face_info.is_eyes_detect_enabled = is_eyes_detect_enabled;
        face_info.is_grin_detect_enabled = is_grin_detect_enabled;
        bool b_found = cvx.detect(img_gray, face_info);

        // enqueue any keyboard events
        wait_and_check_keys();

        // enqueue face/eye found event
        if (b_found)
        {
            app_events.push(FSMEvent(FSMEventCode::E_CVOK));
        }

        // enqueue any state machine timer events
        cvsm.check_timers(app_events);
        psm.check_timers(app_events);

        // apply events to state machines
        // this could generate more events and stuff them in queue
        // there may also be "actions" associated with queued events
        while (app_events.size())
        {
            FSMEvent this_event = app_events.pop();
            cvsm.crank(this_event, app_events);
            psm.crank(this_event, app_events);

            FSMEventCode id = this_event.Code();
            if (action_func_map.count(id))
            {
                tVRevFuncPtr p = action_func_map[id];
                (this->*p)(this_event);
            }
        }

        // update status and displays
        update_fps();
        update_com_status();
        show_monitor_window(img_viewer, face_info, record_sfps);
        check_z();
    }

    // loop was terminated
    // be sure any external action is also halted
    external_action(false);

    // When everything done, release the capture
    vcap.release();
    destroyAllWindows();
}


void AppMain::Go()
{
    std::string haar_cascade_path = "C:\\opencv-3.2.0\\opencv\\build\\etc\\haarcascades\\";

    std::cout << "*** CPOX for Windows 7 64-Bit OS ***" << std::endl;

    std::cout << "Recording path:  ";
    std::cout << "\"" << record_path << "\"" << std::endl;
    if (!is_record_path_ok)
    {
        std::cout << "Path not found" << std::endl;
    }

        /*
            // lazy hard-code for the port settings
            // (used a Keyspan USB-Serial adapter)
            if thread_com.open("/dev/cu.USA19H142P1.1", 9600) :
                std::cout << "Serial Port Opened OK"
            else:
        std::cout << "Failure opening serial port!"
        */

        /*
            if phrase_mgr.load("phrases.txt") :
                std::cout << "Phrase File Loaded OK"
            else :
                std::cout << "Failure loading phrases!"
                */

    // away we go
    if (cvx.load_cascades(haar_cascade_path))
    {
        // start helper tasks
        std::thread tts_task(tts_task_func,
            std::ref(tts_events), std::ref(app_events));
        std::thread com_task(com_task_func,
            std::ref(com_events), std::ref(app_events));

        loop();

        // command helper tasks to halt and wait for them to end
        tts_events.push(FSMEvent(FSMEventCode::E_TASK_HALT));
        com_events.push(FSMEvent(FSMEventCode::E_TASK_HALT));
        tts_task.join();
        com_task.join();

        std::cout << util::GetString(IDS_APP_DONE) << std::endl;
    }
}
