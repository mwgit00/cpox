#include "resource.h"

#include <iostream>

#include "util.h"
#include "AppMain.h"


void AppMain::UIBreak(void)
{
    // set flag to end UI loop and terminate application
    is_looping = false;
}

void AppMain::UIEyes(void)
{
    // toggle eye detection
    is_eyes_detect_enabled = !is_eyes_detect_enabled;
}

void AppMain::UIGrin(void)
{
    // toggle grin detection
    is_grin_detect_enabled = !is_grin_detect_enabled;
}

void AppMain::UITestSay(void)
{
    if (cvsm.is_idle())
    {
        // test retrieval and speaking of next phrase
        // it will be saved for manual recognition step
        s_current_phrase = "this is a test"; ///@TODO -- phrase_mgr.next_phrase()
        tts_events.push(FSMEvent(FSMEventCode::E_TTS_SAY, s_current_phrase));
    }
}

void AppMain::UITestSpeechRec(void)
{
    if (cvsm.is_idle())
    {
        //std::cout << "REC Test:", phrase
        //  thread_rec.post_cmd('hear', phrase)
    }
}

void AppMain::UIHelp(void)
{
    show_help();
}

void AppMain::UITestExt(void)
{
    n_z = 10;
    external_action(true);
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
    zoom_ct = (zoom_ct < ZOOM_STEPS) ? zoom_ct + 1 : zoom_ct;
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


void AppMain::ActionTTSUp(const FSMEvent& r)
{
    std::cout << util::GetString(IDS_APP_TTS_UP) << std::endl;
    is_tts_up = true;
}

void AppMain::ActionTTSSay(const FSMEvent& r)
{
    // pass event to TTS task
    tts_events.push(r);
}

void AppMain::ActionSRPhrase(const FSMEvent& r)
{
    // retrieve next phrase to be repeated
    // and issue command to say it
    // phrase is stashed for upcoming recognition step...
    s_current_phrase = "player will repeat this"; ///@TODO -- FIXME phrase_mgr.next_phrase()
    tts_events.push(FSMEvent(FSMEventCode::E_TTS_SAY, s_current_phrase));
}

void AppMain::ActionSRRec(const FSMEvent& r)
{
    // issue command to recognize a phrase
    // thread_rec.post_cmd('hear', phrase);
}

void AppMain::ActionSRStrikes(const FSMEvent& r)
{
    // update strike display string
    size_t n = r.Data();
    s_strikes = std::string(n, 'X');
}

void AppMain::ActionComXON(const FSMEvent& r)
{
    external_action(true, r.Data());
}

void AppMain::ActionComXOFF(const FSMEvent& r)
{
    external_action(false);
    s_strikes = "";
}

void AppMain::ActionComUp(const FSMEvent& r)
{
    std::cout << util::GetString(IDS_APP_COM_UP) << std::endl;
    is_com_up = true;
}

void AppMain::ActionComAck(const FSMEvent& r)
{
    std::cout << "Ack!" << std::endl;
}
