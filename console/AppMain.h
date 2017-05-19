#ifndef APP_MAIN_H_
#define APP_MAIN_H_

#include <cstdint>
#include <set>
#include <map>

#include "opencv2/imgproc/imgproc.hpp"

#include "MTQueue.hpp"
#include "FaceInfo.h"
#include "FSMLoop.h"
#include "CVMain.h"
#include "PhraseManager.h"
#include "Settings.h"
#include "UDPTask.h"


#define S_COM_NO_ACK    ("--")
#define S_NO_STRIKES    ("---")
#define S_COM_EXT_ON    ("ON")


class AppMain
{
public:

    typedef void (AppMain::*tVVFuncPtr)(void);
    typedef void (AppMain::*tVRevFuncPtr)(const FSMEvent&);

    AppMain();
    virtual ~AppMain();

    static void show_help();

    void check_z();
    void reset_fps();
    void update_fps();
    void update_com_status();
    void record_frame(cv::Mat& frame);
    void external_action(const bool flag, const uint32_t data = 0);
    void show_monitor_window(cv::Mat& img, FaceInfo& rFI, const std::string& rsfps);
    void wait_and_check_keys(void);
    void loop();
    void Go();

    // functions for UI table

    void UIBreak(void);
    void UIEyes(void);
    void UIGrin(void);
    void UIListen(void);
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
    void UISmileU(void);
    void UISmileD(void);
    void UITest1(void);
    void UITest2(void);
    void UITest3(void);
    void UITest4(void);

    // functions for action table
    // actions are generated by state machines or helper tasks
    // actions should NOT generate events

    void ActionSRPhrase(const FSMEvent& r);
    void ActionSRRec(const FSMEvent& r);
    void ActionSRStrikes(const FSMEvent& r);
    void ActionComXON(const FSMEvent& r);
    void ActionComXOFF(const FSMEvent& r);
    void ActionComLevel(const FSMEvent& r);
    void ActionComUp(const FSMEvent& r);
    void ActionComAck(const FSMEvent& r);
    void ActionUDPUp(const FSMEvent& r);
    void ActionUDPSay(const FSMEvent& r);
    void ActionUDPRecVal(const FSMEvent& r);

public:

    Settings cfg;

private:

    CVMain cvx;
    FSMLoop cvsm;
    FSMPhrase psm;
    PhraseManager phrase_mgr;
    
    // high level state info

    bool is_looping;
    bool is_com_up;
    bool is_udp_rx_up;
    bool is_udp_tx_up;
    std::string s_strikes;
    std::string s_current_phrase;
    uint32_t n_z;

    // COM status logic

    bool is_com_blinky_on;
    std::string s_com_ack;
    std::chrono::time_point<std::chrono::steady_clock> t_last_ack;

    // frame recording and frames-per-second (fps) info

    std::string record_sfps;
    bool is_record_path_ok;
    bool is_record_enabled;
    int record_frame_ct;
    int record_clip;
    int record_fps_ct;
    std::chrono::time_point<std::chrono::steady_clock> t_prev;

    // digital zoom, pan, tilt

    int zoom_ct;
    int pan_ct;
    int tilt_ct;

    // application and helper task event queues
    
    tEventQueue app_events;
    tEventQueue com_events;
    tEventQueue udp_events;

    UDPTask udpTask;
    
    std::map<char, AppMain::tVVFuncPtr> ui_func_map;
    std::map<FSMEventCode, AppMain::tVRevFuncPtr> action_func_map;
    std::set<char> cvsm_keys;
};

#endif // APP_MAIN_H_
