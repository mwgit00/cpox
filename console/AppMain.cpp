
#include <iostream>
#include <iomanip>
#include <sstream>

#include "opencv2/highgui/highgui.hpp"

#include "AppMain.h"

/*

AppMain class
- Worker process startup
- Main loop with Face/Eye detection
- Optional Listen-and-Repeat speech recognition mode
- Camera and diagnostic view
- User control via keyboard


void AppMain::make_movie(img_path):
    """Generate an MOV file from a directory of PNG files."""

    // gather file names of frames
    img_files = []
    for (dirpath, dirnames, filenames) in os.walk(img_path):
        img_files.extend(filenames)
        break

    // only consider PNG files
    img_files = [each for each in img_files if each.rfind(".png") > 0]
    if len(img_files) == 0:
        std::cout << "No PNG files found!"
        return

    // determine size of frames
    file_path = os.path.join(img_path, img_files[0])
    size = cv.GetSize(cv.LoadImage(file_path))

    // build movie from separate frames
    // TODO -- may need to change FPS on different systems
    fps = 15
    movie_path = os.path.join(img_path, "movie.mov")
    video_maker = VideoWriter(movie_path,
                                  cv.CV_FOURCC('m', 'p', '4', 'v'),
                                  fps, size)
    if video_maker.isOpened()
        for each in img_files:
        file_path = os.path.join(img_path, each)
            img = imread(file_path)
            video_maker.write(img)
*/

using namespace cv;

AppMain::AppMain()
{
    // OpenCV (B,G,R) named color dictionary
    // (these are purely arbitrary)
    color_name_map["black"] =   SCA_BLACK;
    color_name_map["pink"] =    Scalar(192, 0, 192);
    color_name_map["cyan"] =    Scalar(192, 192, 0);
    color_name_map["gray"] =    SCA_GRAY;
    color_name_map["white"] =   SCA_WHITE;
    color_name_map["yellow"] =  SCA_YELLOW;
    color_name_map["green"] =   SCA_GREEN;
    color_name_map["red"] =     SCA_RED;
    color_name_map["brick"] =   Scalar(64, 64, 128);
    color_name_map["purple"] =  Scalar(128, 64, 64);
    color_name_map["blue"] =    Scalar(192, 0, 0);

    cvsm_keys.insert(KEY_GO);
    cvsm_keys.insert(KEY_HALT);
    cvsm_keys.insert(KEY_LISTEN);

/*
    // worker thread stuff
    thread_tts = poxtts.TTSDaemon()
        thread_rec = poxrec.RECDaemon()
        thread_com = poxcom.Com()
        event_queue = Queue.Queue()

        // execution stuff
        phrase_mgr = poxutil.PhraseManager()
        roi = None
*/
    
    // state stuff best suited to top-level app
    b_eyes = true;
    b_grin = false;
    s_strikes = "";
    phrase = "";
    n_z = 0;

    // frame capture and recording
    record_ok = false; // os.path.isdir(record_path)
    record_enable = false;
    record_ct = 0;
    record_clip = 0;
    record_k = 0;
    record_sfps = "???";
    record_path = ""; // os.path.join(os.path.dirname(os.path.abspath(__file__)), "movie")

    // scale the face detection ROI
    // will chop a percentage from top/bottom and left/right
    // can only use values in the range 0.0 - 0.5
    roi_perc_h = 0.1;
    roi_perc_w = 0.2;
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
    record_k = 0;
    record_sfps = "???";
}

void AppMain::update_fps()
{
    // recalculate frames-per-second after this many frames
    const int FPS_FRAME_CT = 20;
    record_k += 1;
    if (record_k == FPS_FRAME_CT)
    {
        // calculate elapsed time for 100 frames
        std::chrono::time_point<std::chrono::steady_clock> t_curr =
            std::chrono::high_resolution_clock::now();
        double tx = std::chrono::duration<double>(t_curr - t_prev).count();

        // convert to frame rate string
        double fps = static_cast<double>(record_k) / tx;
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(1) << fps;
        record_sfps = oss.str();

        // reset for next measurement
        t_prev = t_curr;
        record_k = 0;
    }
}

void AppMain::record_frame(Mat& frame, const std::string& name_prefix)
{
#if 0
    // record frames to sequentially numbered files if enabled
    if (record_ok && record_enable)
    {
        std::string file_name = name_prefix;
        if (file_name.length() == 0)
        {
            file_name = "frame";
        }
        
        file_name += "_";
        file_name += str(record_clip).zfill(2);
        file_name += "_";
        file_name += str(record_ct).zfill(5);
        file_name += ".png";
        file_path = os.path.join(record_path, file_name);
        record_ct += 1;
        imwrite(file_path, frame);
    }
#endif
}

cv::Rect AppMain::get_roi(const int h, const int w) const
{
/*
Given source image dimensions, returns X and Y
coordinates for region - of - interest based on
percentages for chopping top / bottom and left / right.

:param h : Height of source image
: param w : Width of source image
: return : (y0, y1, x0, x1)
*/
    double rh = roi_perc_h;
    double rw = roi_perc_w;
    return Rect(int(rh * h), int((1.0 - rh) * h), int(rw * w), int((1.0 - rw) * w));
}

void AppMain::external_action(const bool flag, const uint32_t data)
{
    // sends command to an external serial device.
    if (flag)
    {
        // configure digital pin as output and turn on
     //   thread_com.post_cmd("dig0_cfg", "0")
       //     thread_com.post_cmd("dig0_io", "1")
        std::cout << "EXT ON " << data << std::endl;
    }
    else
    {
        // turn off digital pin and configure as input
        //thread_com.post_cmd("dig0_io", "0")
          //  thread_com.post_cmd("dig0_cfg", "1")
            std::cout << "EXT OFF" << std::endl;
    }
}

void AppMain::show_help()
{
    // press '?' while monitor has focus
    // to see this menu
    std::cout << "? - Display help." << std::endl;
    std::cout << KEY_EYES   << " - Toggle eye detection." << std::endl;
    std::cout << KEY_GRIN   << " - Toggle smile detection." << std::endl;
    std::cout << KEY_GO     << " - Go. Restarts monitoring." << std::endl;
    std::cout << KEY_HALT   << " - Halt. Stops monitoring and any external action." << std::endl;
    std::cout << KEY_LISTEN << " - Start scripted speech mode.  Only valid when monitoring." << std::endl;
    std::cout << "M - Make MOV movie file from recorded video frames." << std::endl;
    std::cout << KEY_SAY    << " - (Test) Say next phrase from file." << std::endl;
    std::cout << "r - (Test) Recognize phrase that was last spoken." << std::endl;
    std::cout << KEY_QUIT   << " - Quit." << std::endl;
    std::cout << "V - Toggle video recording." << std::endl;
    std::cout << "Z - (Test) Activate external output for half-second." << std::endl;
    std::cout << "ESC - Quit." << std::endl;
}

void AppMain::show_monitor_window(cv::Mat& img, FaceInfo& rFI, const std::string& rsfps)
{
    // update display items
    Scalar status_color = color_name_map[cvsm.Snapshot("color")];
    std::string s_label = cvsm.Snapshot("label");

    Size sz = img.size();
    Rect roi = get_roi(sz.height, sz.width);

    // draw face boxes and face ROI
    Mat img_final = img;

    rFI.rgb_draw_boxes(img_final);
#if 0
    rectangle(img_final, roi, color_name_map["cyan"]);
#endif

    int wn = 54;  // width of status boxes
    int hn = 20;  // height of status boxes

    // draw status label in upper left
    // along with status color
    rectangle(img_final, Rect(0, 0, wn, hn), status_color, CV_FILLED);
    rectangle(img_final, Rect(0, 0, wn, hn), color_name_map["white"]);
    putText(img_final, s_label, Point(10, 14), FONT_HERSHEY_PLAIN, 1.0,
        color_name_map["white"], 2);

    // mode state icon box
    rectangle(img_final, Rect(Point(0, hn), Point(wn, hn * 2)), color_name_map["purple"], CV_FILLED);
    rectangle(img_final, Rect(Point(0, hn), Point(wn, hn * 2)), color_name_map["white"]);

    // strike count display
    std::string speech_mode_color = "gray"; // FIXME cvsm.psm.Snapshot("color");
    rectangle(img_final, Rect(Point(0, hn * 2), Point(wn, hn * 3)),
        color_name_map[speech_mode_color], CV_FILLED);
    rectangle(img_final, Rect(Point(0, hn * 2), Point(wn, hn * 3)),
        color_name_map["white"]);
    putText(img_final, s_strikes, Point(10, hn * 2 + 14),
        FONT_HERSHEY_PLAIN, 1.0, color_name_map["white"], 2);

    // frames per second and recording status
    std::string fps_color = (record_enable) ? "red" : "black";
    rectangle(img_final, Rect(Point(0, hn * 3), Point(wn, hn * 4)),
        color_name_map[fps_color], CV_FILLED);
    rectangle(img_final, Rect(Point(0, hn * 3), Point(wn, hn * 4)),
        color_name_map["white"]);
    putText(img_final, rsfps, Point(10, hn * 3 + 14),
        FONT_HERSHEY_PLAIN, 1.0, color_name_map["white"], 2);

    /*
        // draw speech recognition progress (timeout) bar if active
        // just a black rectangle that gets filled with gray blocks
        // there's a yellow warning bar at ideal timeout time
        x = int(cvsm.Snapshot("prog"])
        if x > 0:
    rec_sec = int(poxfsm.SMPhrase.REC_TIMEOUT_SEC)
        wb = 10
        x1 = wn
        x2 = wn + (rec_sec - x) * wb
        x3 = wn + rec_sec * wb
        xtrg = x1 + 12 * wb  // see poxrec.py
        rectangle(img_final, (x1, 0), (x2, hn), color_name_map["gray"],
            cv.CV_FILLED)
        rectangle(img_final, (x2, 0), (x3, hn), color_name_map["black"],
            cv.CV_FILLED)
        line(img_final, (xtrg, 0), (xtrg, hn), color_name_map["yellow"])
        rectangle(img_final, (x1, 0), (x3, hn), color_name_map["white"])
        */

    // draw eye detection state indicator (pair of eyes)
    if (b_eyes)
    {
        int e_y = 28;
        int e_x = 8;
        int e_dx = 8;
        circle(img_final, Point(e_x, e_y), 3, color_name_map["white"],
            CV_FILLED);
        circle(img_final, Point(e_x + e_dx, e_y), 3, color_name_map["white"],
            CV_FILLED);
        circle(img_final, Point(e_x, e_y), 1, color_name_map["black"],
            CV_FILLED);
        circle(img_final, Point(e_x + e_dx, e_y), 1, color_name_map["black"],
            CV_FILLED);
    }

    // draw grin detection state indicator (curve like a grin)
    if (b_grin)
    {
        int g_x = 34;
        int g_y = 28;
        ellipse(img_final, Point(g_x, g_y), Point(5, 3), 0, 0, 180,
            color_name_map["white"], 2);
    }

    // record frame if enabled and update monitor
    record_frame(img_final, "img");
    imshow("POX Monitor", img_final);
}

bool AppMain::wait_and_check_keys(tListEvent& event_list)
{
    bool result = true;

    // this is where all key input comes from
    // key press that affects state machine will be stuffed in event
    // that event will be handled at next iteration

    char key = waitKey(1);

    if ((key == KEY_ESC) || (key == KEY_QUIT))
    {
        result = false;
    }
    else if (key == KEY_EYES)
    {
        // toggle eye detection
        b_eyes = !b_eyes;
    }
    else if (key == KEY_GRIN)
    {
        // toggle grin detection
        b_grin = !b_grin;
    }
    else if (cvsm_keys.count(key))
    {
        event_list.push_back(FSMEvent(FSMEventCode::E_KEY, key));
    }
    else if (key == KEY_SAY)
    {
        if (cvsm.is_idle())
        {
            // test retrieval and speaking of next phrase
            // it will be saved for manual recognition step
         //   phrase = phrase_mgr.next_phrase()
           //     thread_tts.post_cmd('say', phrase)
        }
    }
    else if (key == 'r')
    {
        if (cvsm.is_idle())
        {
            //std::cout << "REC Test:", phrase
              //  thread_rec.post_cmd('hear', phrase)
        }
    }
    else if (key == '?')
    {
        show_help();
    }
    else if (key == 'Z')
    {
        n_z = 10;
        external_action(true);
    }
    else if (key == 'V')
    {
        if (record_ok)
        {
            if (record_enable)
            {
                // enabled to disabled
                record_enable = false;
            }
            else
            {
                // disabled to enabled, reset frame ct
                record_enable = true;
                record_clip += 1;
                record_ct = 0;
            }
        }
    }
    else if (key == 'M')
    {
        std::cout << "Begin making movie" << std::endl;
        //make_movie(record_path);
        std::cout << "Finished" << std::endl;
        reset_fps();
    }

    return result;
}

bool AppMain::loop()
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
        std::cout << "Camera Device failed to open." << std::endl;
        return false;
    }

    // this may need to change depending on camera
    double img_scale = 0.5;

    // this must persist between iterations
    tListEvent events;

    reset_fps();

    while (true)
    {
        Mat img;
        Mat img_small;
        Mat img_gray;
        FaceInfo face_info;

        // process images frame-by-frame
        // grab image, downsize, extract ROI, run detection
        // b_found will be result of face/eye/grin detection
        // boxes have data for drawing rectangles for what was detected
        vcap >> img;

        resize(img, img_small, Size(), img_scale, img_scale);
        cvtColor(img_small, img_gray, COLOR_BGR2GRAY);

        Size sz = img_small.size();
        Rect roi = get_roi(sz.height, sz.width); // FIXME
        // Mat imgx = img_small(roi);
        
        face_info.is_eyes_enabled = b_eyes;
        face_info.is_grin_enabled = b_grin;
        bool b_found = cvx.detect(img_gray, face_info);

        // propagate face/eye found event
        if (b_found)
        {
            events.push_back(FSMEvent(FSMEventCode::E_CVOK));
        }

        /*
            // poll to see if workers sent any messages
            while !event_queue.empty()
                x = event_queue.get()
                event_queue.task_done()
                stokens = x.split()
                if stokens[0] == poxtts.POX_TTS:
        events.append(poxfsm.SMEvent(FSMEventCode::E_SDONE))
                else if (stokens[0] == poxrec.POX_REC :
        if stokens[1] == 'init' :
            // just std::cout << out initialization result
            std::cout << " ".join(stokens[2:])
        else if (cvsm.is_idle()
            // probably running a test
            // so just std::cout << message
            std::cout << x
        else:
        // second token is 'True' or 'False'
        // state machine will ack with strike count
        flag = eval(stokens[1])
            rdone = poxfsm.SMEvent(FSMEventCode::E_RDONE, flag)
            events.append(rdone)
                else if (stokens[0] == poxcom.POX_COM:
        std::cout << stokens
        */

        // event list may have worker thread events and detection OK event
        // add any state machine timer events to event list
        // then apply events to state machine
        tListEvent outputs;
        cvsm.check_timers(events);
        for (const auto& this_event : events)
        {
            cvsm.crank(this_event, outputs);
        }
        events.clear();

        // handle any output actions produced by state machine
        for (const auto& this_event : outputs)
        {
            if (this_event.Code() == FSMEventCode::E_SAY)
            {
                // issue command to say a phrase
                // FIXME thread_tts.post_cmd('say', action.data)
            }
            else if (this_event.Code() == FSMEventCode::E_SAY_REP)
            {
                // retrieve next phrase to be repeated
                // and issue command to say it
                // phrase is stashed for upcoming recognition step...
             //   phrase = phrase_mgr.next_phrase()
               //     thread_tts.post_cmd('say', phrase)
            }
            else if (this_event.Code() == FSMEventCode::E_SRGO)
            {
                // issue command to recognize a phrase
                // thread_rec.post_cmd('hear', phrase);
            }
            else if (this_event.Code() == FSMEventCode::E_SRACK)
            {
                // update strike display string
                // propagate FAIL message if limit reached
             //   s_strikes = "X" * action.data
               //     if action.data == 3 :
               // events.append(poxfsm.SMEvent(FSMEventCode::E_SRFAIL))
            }
            else if (this_event.Code() == FSMEventCode::E_XON)
            {
                external_action(true, this_event.Data());
            }
            else if (this_event.Code() == FSMEventCode::E_XOFF)
            {
                external_action(false);
                s_strikes = "";
            }
        }

        // update displays
        update_fps();
        show_monitor_window(img_small, face_info, record_sfps);
        check_z();

        // final step is to check keys
        // key events will be handled next iteration
        // loop might be terminated here if check returns False
        if (!wait_and_check_keys(events))
        {
            break;
        }
    }

    // loop was terminated
    // be sure any external action is also halted
    external_action(false);

    // When everything done, release the capture
    vcap.release();
    destroyAllWindows();
    return true;
}


void AppMain::Go()
{
    std::string haar_cascade_path = "C:\\opencv-3.2.0\\opencv\\build\\etc\\haarcascades\\";

    std::cout << "*** CPOX ***" << std::endl;
    std::cout << "WIN64" << std::endl;
    if (!record_ok)
    {
        std::cout << "Recording disabled.  Path not found:  \"";
        std::cout << record_path;
        std::cout << "\"" << std::endl;
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
        /*
        thread_tts.start(event_queue)
        thread_rec.start(event_queue)
        thread_com.start(event_queue)
        */
        loop();
        std::cout << "DONE" << std::endl;
    }
}
