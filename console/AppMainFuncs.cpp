#include <iostream>
#include "AppMain.h"


void AppMain::UIBreak(void)
{
    // set flag to end UI loop and terminate application
    b_looping = false;
}

void AppMain::UIEyes(void)
{
    // toggle eye detection
    b_eyes = !b_eyes;
}

void AppMain::UIGrin(void)
{
    // toggle grin detection
    b_grin = !b_grin;
}

void AppMain::UITestSay(void)
{
    if (cvsm.is_idle())
    {
        // test retrieval and speaking of next phrase
        // it will be saved for manual recognition step
        phrase = "this is a test"; ///@TODO -- phrase_mgr.next_phrase()
        tts_task.post_event(FSMEvent(FSMEventCode::E_TTS_SAY, phrase));
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



void AppMain::ActionTTSSay(const FSMEvent& r)
{
    // pass event to TTS task
    tts_task.post_event(r);
}

void AppMain::ActionSRPhrase(const FSMEvent& r)
{
    // retrieve next phrase to be repeated
    // and issue command to say it
    // phrase is stashed for upcoming recognition step...
    phrase = "repeat this phrase"; ///@TODO -- FIXME phrase_mgr.next_phrase()
    tts_task.post_event(FSMEvent(FSMEventCode::E_TTS_SAY, phrase));
}

void AppMain::ActionSRRec(const FSMEvent& r)
{
    // issue command to recognize a phrase
    // thread_rec.post_cmd('hear', phrase);
}

void AppMain::ActionSRStrikes(const FSMEvent& r)
{
    // update strike display string
    // propagate FAIL event if limit reached
    size_t n = r.Data();
    s_strikes = std::string(n, 'X');
    if (n == 3)
    {
        app_events.push(FSMEvent(FSMEventCode::E_SR_FAIL));
    }
}

void AppMain::ActionSRStop(const FSMEvent& r)
{
    app_events.push(FSMEvent(FSMEventCode::E_SR_STOP));
}


void AppMain::ActionXON(const FSMEvent& r)
{
    external_action(true, r.Data());
}

void AppMain::ActionXOFF(const FSMEvent& r)
{
    external_action(false);
    s_strikes = "";
}
