#ifndef APP_MAIN_H_
#define APP_MAIN_H_

#include <cstdint>
#include <set>

#include "opencv2/imgproc/imgproc.hpp"

#include "MTQueue.hpp"
#include "FaceInfo.h"
#include "FSMLoop.h"
#include "CVMain.h"

#define ZOOM_STEPS  (20)


class AppMain
{
public:

    typedef void (AppMain::*tUIFuncPtr)(void);

    AppMain();
    virtual ~AppMain();

    static void show_help();

    void check_z();
    void reset_fps();
    void update_fps();
    void record_frame(cv::Mat& frame);
    void external_action(const bool flag, const uint32_t data = 0);
    void show_monitor_window(cv::Mat& img, FaceInfo& rFI, const std::string& rsfps);
    void wait_and_check_keys(tListEvent& event_list);
    void loop();
    void Go();

    void UIBreak(void);
    void UIEyes(void);
    void UIGrin(void);
    void UITestSay(void);
    void UITestSpeechRec(void);
    void UIHelp(void);
    void UITestExt(void);
    void UIRecord(void);
    void UIMakeMovie(void);
    void UIZoomIn(void);
    void UIZoomOut(void);
    void UIPanL(void);
    void UIPanR(void);
    void UITiltU(void);
    void UITiltD(void);
    void UIResetZoom(void);
    void UIResetPanTilt(void);

private:

    CVMain cvx;
    FSMLoop cvsm;
    
    // high level state info

    bool b_looping;
    bool b_eyes;
    bool b_grin;
    std::string s_strikes;
    std::string phrase;
    uint32_t n_z;

    // frame recording info

    std::string record_sfps;
    std::string record_path;
    bool record_ok;
    bool record_enable;
    int record_ct;
    int record_clip;
    int record_k;
    
    // digital zoom, pan, tilt

    int zoom_ct;
    int pan_ct;
    int tilt_ct;

    std::chrono::time_point<std::chrono::steady_clock> t_prev;
    std::map<char, AppMain::tUIFuncPtr> ui_func_map;
    std::set<char> cvsm_keys;
};

#endif // APP_MAIN_H_
