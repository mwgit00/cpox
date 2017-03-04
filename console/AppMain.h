#ifndef APP_MAIN_H_
#define APP_MAIN_H_

#include <cstdint>
#include <set>

#include "opencv2/imgproc/imgproc.hpp"

#include "FaceInfo.h"
#include "FSMLoop.h"
#include "CVMain.h"

typedef std::map<std::string, cv::Scalar> tMapStrBGR;

class AppMain
{
public:

    AppMain();
    virtual ~AppMain();

    void check_z();
    void reset_fps();
    void update_fps();
    void record_frame(cv::Mat& frame, const std::string& name_prefix);

    cv::Rect get_roi(const int h, const int w) const;

    void external_action(const bool flag, const uint32_t data = 0);

    void show_monitor_window(cv::Mat& img, FaceInfo& rFI, const std::string& rsfps);
    bool wait_and_check_keys(tListEvent& event_list);

    bool loop();
    void Go();

    static void show_help();

private:

    CVMain cvx;
    FSMLoop cvsm;
    
	tMapStrBGR color_name_map;

    bool b_eyes;
    bool b_grin;
    std::string s_strikes;
    std::string phrase;
    uint32_t n_z;

    bool record_ok;
    bool record_enable;
    int record_ct;
    int record_clip;
    int record_k;
    std::string record_sfps;
    std::string record_path;
    
	std::chrono::time_point<std::chrono::steady_clock> t_prev;

    double roi_perc_h;
    double roi_perc_w;

    std::set<char> cvsm_keys;
};

#endif // APP_MAIN_H_
