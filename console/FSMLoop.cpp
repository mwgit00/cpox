#include <sstream>
#include "FSMLoop.h"
#include "defs.h"


FSMLoop::FSMLoop() :
    state(STATE_IDLE),
    cv_timer(),
    level(0u)
{
    snapshot.sec = 0;
    snapshot.color = SCA_BLACK;
    snapshot.label = "IDLE";
    snapshot.prog = 0;
}

FSMLoop::~FSMLoop()
{
}

bool FSMLoop::is_idle() const
{
    return (state == STATE_IDLE);
}

void FSMLoop::_to_norm()
{
    // helper for transition to norm state (no outputs generated)
    cv_timer.start(NORM_TIMEOUT_SEC);
    state = STATE_NORM;
}

void FSMLoop::_to_act(tEventQueue& rq)
{
    // helper for transition to ACT state
    
    cv_timer.start(ACT_TIMEOUT_SEC);
    state = STATE_ACT;
    
    // increment level unless it has reached maximum
    if (level < APP_MAX_LEVEL)
    {
        level += 1;
    }

    // stop phrase machine
    // turn on external action (pass along new level data)
    rq.push(FSMEvent(FSMEventCode::E_SR_STOP));
    rq.push(FSMEvent(FSMEventCode::E_COM_XON, level));
}

void FSMLoop::check_timers(tEventQueue& rq)
{
    // update the snapshot FIRST
    // to prevent countdown time of 0 appearing briefly before a state change
    // then update the timer

    snapshot.sec = cv_timer.sec();

    if (state == STATE_IDLE)
    {
        snapshot.color = SCA_BLACK;
        snapshot.label = "IDLE";
    }
    else if (state == STATE_INH)
    {
        std::ostringstream oss;
        oss << snapshot.sec;
        snapshot.color = SCA_BLUE;
        snapshot.label = oss.str();
    }
    else if (state == STATE_NORM)
    {
        snapshot.color = SCA_GREEN_MED;
        snapshot.label = "OK";
    }
    else if (state == STATE_WARN)
    {
        std::ostringstream oss;
        oss << cv_timer.sec();
        snapshot.color = SCA_YELLOW_MED;
        snapshot.label = oss.str();
    }
    else if (state == STATE_ACT)
    {
        snapshot.color = SCA_RED;
        snapshot.label = "FAIL";
    }

    uint32_t sec;
    bool flag = cv_timer.update(sec);
    if (flag)
    {
        rq.push(FSMEvent(FSMEventCode::E_TMR_CV));
    }
}


void FSMLoop::crank(const FSMEvent& this_event, tEventQueue& rq)
{
    // CHECK FOR HIGH-PRIORITY HALT
    // in any state other than idle and halt event occurs
    // - stop timer
    // - reset level
    // - go back to idle
    // - stop everything else related to monitoring
    if (state != STATE_IDLE)
    {
        if (this_event.Code() == FSMEventCode::E_KEY)
        {
            if (this_event.Data() == KEY_HALT)
            {
                cv_timer.stop();
                level = 0u;
                state = STATE_IDLE;

                // turn off phrase state machine
                // turn off any external action
                // announce halt
                rq.push(FSMEvent(FSMEventCode::E_SR_STOP));
                rq.push(FSMEvent(FSMEventCode::E_COM_XOFF));
                rq.push(FSMEvent(FSMEventCode::E_TTS_SAY, "session halted"));
                
                ///////
                return;
                ///////
            }
        }
    }
    
    if (state == STATE_IDLE)
    {
        if (this_event.Code() == FSMEventCode::E_KEY)
        {
            if (this_event.Data() == KEY_GO)
            {
                cv_timer.start(FSMLoop::INH_TIMEOUT_SEC);
                state = STATE_INH;
                // announce countdown has started
                rq.push(FSMEvent(FSMEventCode::E_TTS_SAY, "get ready"));
            }
        }
    }
    else if (state == STATE_INH)
    {
        if (this_event.Code() == FSMEventCode::E_TMR_CV)
        {
            _to_norm();
            // reset speech rec. SM so it can be started again
            // announce start of monitoring
            rq.push(FSMEvent(FSMEventCode::E_SR_RESET));
            rq.push(FSMEvent(FSMEventCode::E_TTS_SAY, "go"));
        }
    }
    else if (state == STATE_NORM)
    {
        if (this_event.Code() == FSMEventCode::E_CVOK)
        {
            _to_norm();
        }
        else if (this_event.Code() == FSMEventCode::E_TMR_CV)
        {
            cv_timer.start(FSMLoop::WARN_TIMEOUT_SEC);
            state = STATE_WARN;
        }
        else if (this_event.Code() == FSMEventCode::E_SR_FAIL)
        {
            _to_act(rq);
        }
    }
    else if (state == STATE_WARN)
    {
        if (this_event.Code() == FSMEventCode::E_CVOK)
        {
            _to_norm();
        }
        else if (this_event.Code() == FSMEventCode::E_TMR_CV)
        {
            _to_act(rq);
        }
        else if (this_event.Code() == FSMEventCode::E_SR_FAIL)
        {
            _to_act(rq);
        }
    }
    else if (state == STATE_ACT)
    {
        if (this_event.Code() == FSMEventCode::E_TMR_CV)
        {
            _to_norm();
            // restart phrase machine
            // turn off any external action
            rq.push(FSMEvent(FSMEvent(FSMEventCode::E_SR_RESTART)));
            rq.push(FSMEvent(FSMEventCode::E_COM_XOFF));
        }
    }
}

const FSMSnapShot& FSMLoop::Snapshot() const
{
    return snapshot;
}
