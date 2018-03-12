#include "resource.h"

#include <iostream>
#include <iomanip>

#include "util.h"
#include "AppMain.h"
#include "COMTask.h"

#define MAX_SMILE_THR   (50)


void AppMain::UIBreak(void)
{
    // set flag to end UI loop and terminate application
    is_looping = false;
}

void AppMain::UIEyes(void)
{
    if (cvsm.is_idle())
    {
        // toggle eye detection
        cfg.loop.eyes_flag = (cfg.loop.eyes_flag) ? 0 : 1;
    }
}

void AppMain::UIGrin(void)
{
    if (cvsm.is_idle())
    {
        // toggle grin detection
        cfg.loop.smile_flag = (cfg.loop.smile_flag) ? 0 : 1;
    }
}

void AppMain::UIListen(void)
{
    if (cvsm.is_idle())
    {
        // toggle listen-and-repeat mode
        cfg.loop.listen_flag = (cfg.loop.listen_flag) ? 0 : 1;
        psm.set_enabled(cfg.loop.listen_flag ? true : false);
    }
}

void AppMain::UITestSay(void)
{
    if (cvsm.is_idle())
    {
        // test retrieval and speaking of next phrase
        // it will be loaded into Speech Manager
        // the Speech Manager will do TTS or play associated WAV file if it exists
        
        PhraseManager::T_phrase_info current_phrase = phrase_mgr.next_phrase();
        s_current_phrase = current_phrase.text;

        udp_events.push(FSMEvent(FSMEventCode::E_UDP_LOAD, s_current_phrase));
        if (!current_phrase.wav.empty())
        {
            std::cout << "WAV " << current_phrase.wav << std::endl;
            udp_events.push(FSMEvent(FSMEventCode::E_UDP_WAV, current_phrase.wav));
        }
        else
        {
            std::cout << "TTS " << s_current_phrase << std::endl;
            udp_events.push(FSMEvent(FSMEventCode::E_UDP_REPEAT));
        }
    }
}

void AppMain::UITestSpeechRec(void)
{
    if (cvsm.is_idle())
    {
        // test recognition of last loaded phrase
        std::cout << "REC " << s_current_phrase << std::endl;
        udp_events.push(FSMEvent(FSMEventCode::E_UDP_REC));
    }
}

void AppMain::UIHelp(void)
{
    show_help();
}

void AppMain::UITestExt(void)
{
    n_z = 10;
    external_action(true, 100);
}

void AppMain::UIRecord(void)
{
    if (is_record_path_ok)
    {
        if (is_record_enabled)
        {
            // enabled to disabled
            is_record_enabled = false;
        }
        else
        {
            // disabled to enabled, reset frame ct
            is_record_enabled = true;
            record_clip += 1;
            record_frame_ct = 0;
        }
    }
}

void AppMain::UIMakeMovie(void)
{
    std::cout << "(NOT SUPPORTED YET) Begin making movie" << std::endl;
    //make_movie(record_path);
    std::cout << "Finished" << std::endl;
    reset_fps();
}

void AppMain::UIZoomIn(void)
{
    // zoom in
    zoom_ct = (zoom_ct < cfg.app.zoom_steps) ? zoom_ct + 1 : zoom_ct;
}

void AppMain::UIZoomOut(void)
{
    // zoom out
    if (zoom_ct > 0)
    {
        zoom_ct--;
        
        // pan range depends on zoom
        // so if zooming out then must adjust pan/tilt counts
        // their absolute values cannot be greater than zoom count
        
        if (abs(pan_ct) > zoom_ct)
        {
            pan_ct = (pan_ct < 0) ? pan_ct + 1 : pan_ct - 1;
        }
        
        if (abs(tilt_ct) > zoom_ct)
        {
            tilt_ct = (tilt_ct < 0) ? tilt_ct + 1 : tilt_ct - 1;
        }
    }
}

void AppMain::UIPanL(void)
{
    // pan left
    pan_ct = (pan_ct < zoom_ct) ? pan_ct + 1 : pan_ct;
}

void AppMain::UIPanR(void)
{
    // pan right
    pan_ct = (pan_ct > -zoom_ct) ? pan_ct - 1 : pan_ct;
}

void AppMain::UITiltU(void)
{
    // digital tilt up
    tilt_ct = (tilt_ct < zoom_ct) ? tilt_ct + 1 : tilt_ct;
}

void AppMain::UITiltD(void)
{
    // digital tilt down
    tilt_ct = (tilt_ct > -zoom_ct) ? tilt_ct - 1 : tilt_ct;
}

void AppMain::UIResetZoom(void)
{
    // reset zoom to 1x
    // must also reset pan and tilt
    zoom_ct = 0;
    pan_ct = 0;
    tilt_ct = 0;
}

void AppMain::UIResetPanTilt(void)
{
    pan_ct = 0;
    tilt_ct = 0;
}

void AppMain::UITest1(void)
{
    // fake SR failure
    app_events.push(FSMEvent(FSMEventCode::E_SR_RESULT, 0));
}

void AppMain::UITest2(void)
{
    // fake SR success
    app_events.push(FSMEvent(FSMEventCode::E_SR_RESULT, 1));
}

void AppMain::UITest3(void)
{
    // set ext device minimum output level (limited in configuration)
    app_events.push(FSMEvent(FSMEventCode::E_COM_LEVEL, cfg.loop.min_level));
}

void AppMain::UITest4(void)
{
    // set ext device maximum output level (limited in configuration)
    app_events.push(FSMEvent(FSMEventCode::E_COM_LEVEL, cfg.loop.max_level));
}

void AppMain::UISmileU(void)
{
    // increase smile threshold by 2 units until max reached
    int& ri = cfg.loop.smile_thr;
    ri = (ri < MAX_SMILE_THR) ? ri + 2 : ri;
}

void AppMain::UISmileD(void)
{
    // decrease smile threshold by 2 units until 0 reached
    int& ri = cfg.loop.smile_thr;
    ri = (ri > 0) ? ri - 2 : ri;
}


void AppMain::ActionSRPhrase(const FSMEvent& r)
{
    // retrieve next phrase to be repeated
    // and issue commands to load it and repeat it
    // phrase is stashed for upcoming recognition step...

    PhraseManager::T_phrase_info current_phrase = phrase_mgr.next_phrase();
    s_current_phrase = current_phrase.text;

    udp_events.push(FSMEvent(FSMEventCode::E_UDP_LOAD, s_current_phrase));
    if (!current_phrase.wav.empty())
    {
        udp_events.push(FSMEvent(FSMEventCode::E_UDP_WAV, current_phrase.wav));
    }
    else
    {
        udp_events.push(FSMEvent(FSMEventCode::E_UDP_REPEAT));
    }

}

void AppMain::ActionSRRec(const FSMEvent& r)
{
    // issue command to recognize a phrase
    udp_events.push(FSMEvent(FSMEventCode::E_UDP_REC));
}

void AppMain::ActionSRStrikes(const FSMEvent& r)
{
    // update strike display string
    size_t n = r.Data();
    s_strikes = std::string(n, 'X') + std::string((3 - n), '-');
}

void AppMain::ActionComXON(const FSMEvent& r)
{
    external_action(true, r.Data());
}

void AppMain::ActionComXOFF(const FSMEvent& r)
{
    // can clear strike counter after external device is turned off
    external_action(false);
    s_strikes = S_NO_STRIKES;
}

void AppMain::ActionComLevel(const FSMEvent& r)
{
    if (is_com_up)
    {
        com_events.push(r);
    }
}

void AppMain::ActionComUp(const FSMEvent& r)
{
    is_com_up = r.Data() ? true : false;
    int ids = is_com_up ? IDS_APP_COM_UP : IDS_APP_COM_DOWN;
    std::cout << util::GetString(ids) << std::endl;
}

void AppMain::ActionComAck(const FSMEvent& r)
{
    t_last_ack = std::chrono::high_resolution_clock::now();
    is_com_blinky_on = !is_com_blinky_on;
    if (r.Str().length() >= 3)
    {
        // character at index 1 is command
        // character at index 2 is output level [a-y] or ?
        // convert that to a string with the integer value
        
        char c_cmd = r.Str().at(1);
        char c_val = r.Str().at(2);
        
        if (c_cmd == CMD_OUTPUT_ON)
        {
            // report when on
            s_com_ack = S_COM_EXT_ON;
        }
        else if ((c_val >= CMD_VAL_MIN) && (c_val <= CMD_VAL_MAX))
        {
            // otherwise report level
            int n_ack_level = static_cast<int>(c_val - CMD_VAL_MIN) + 1;
            std::ostringstream oss;
            oss << std::setw(2) << std::setfill(' ') << n_ack_level;
            s_com_ack = oss.str();
        }
        else
        {
            s_com_ack = S_COM_NO_ACK;
        }
    }
    else
    {
        s_com_ack = S_COM_NO_ACK;
    }
}

void AppMain::ActionUDPUp(const FSMEvent& r)
{
    int ids = IDS_APP_UNKNOWN;
    
    switch (r.Data())
    {
    case (UDP_RX_MASK | 0):
        ids = IDS_APP_UDP_RX_DOWN;
        break;
    case (UDP_RX_MASK | 1):
        ids = IDS_APP_UDP_RX_UP;
        break;
    case (UDP_TX_MASK | 0):
        ids = IDS_APP_UDP_TX_DOWN;
        break;
    case (UDP_TX_MASK | 1):
        ids = IDS_APP_UDP_TX_UP;
        break;
    default:
        break;
    }

    std::cout << util::GetString(ids) << std::endl;
}

void AppMain::ActionUDPSay(const FSMEvent& r)
{
    // pass event to UDP task
    udp_events.push(r);
}

void AppMain::ActionUDPRecVal(const FSMEvent& r)
{
    // this is just for testing in the app
    if (psm.is_idle() || psm.is_stopped())
    {
        std::cout << "REC " << r.Data() << std::endl;
    }
}
