#include <sstream>
#include "FSMLoop.h"
#include "defs.h"


FSMLoop::FSMLoop() :
    state(STATE_IDLE),
    cv_timer(),
    level(0u)
{
    snapshot["color"] = "black";
    snapshot["label"] = "IDLE";
    snapshot["prog"] = "0";
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

void FSMLoop::_to_act(tListEvent& temp_outputs)
{
    // helper for transition to ACT state
    cv_timer.start(ACT_TIMEOUT_SEC);
    state = STATE_ACT;
    
    // increment  level unless it has reached maximum
    if (level < APP_MAX_LEVEL)
    {
        level += 1;
    }

    // stop phrase machine
    // FIXME temp_outputs.extend(psm.crank(SMEvent(FSMEventCode::E_STOP)))
    
    // turn on external action (pass along new level data)
    temp_outputs.push_back(FSMEvent(FSMEventCode::E_XON, level));
}

void FSMLoop::check_timers(tListEvent& tmr_outputs)
{
    // first update snapshot that is used for display
    // because it has some timer-based stuff

    ///@TODO
#if 0 // FIXME
    // get data for progress bar for speech recognition
    if psm.state == SMPhrase.STATE_REC:
    snapshot["prog"] = str(psm.timer.sec())
    else:
    snapshot["prog"] = "0"
#endif

    // get data for status indicator
    if (state == STATE_IDLE)
    {
        snapshot["color"] = "black";
        snapshot["label"] = "IDLE";
    }
    else if (state == STATE_INH)
    {
        std::ostringstream oss;
        oss << cv_timer.sec();
        snapshot["color"] = "blue";
        snapshot["label"] = oss.str();
    }
    else if (state == STATE_NORM)
    {
        snapshot["color"] = "green";
        snapshot["label"] = "OK";
    }
    else if (state == STATE_WARN)
    {
        std::ostringstream oss;
        oss << cv_timer.sec();
        snapshot["color"] = "yellow";
        snapshot["label"] = oss.str();
    }
    else if (state == STATE_ACT)
    {
        snapshot["color"] = "red";
        snapshot["label"] = "FAIL";
    }

    // handle own timeouts first
    uint32_t sec;
    bool flag = cv_timer.update(sec);
    if (flag)
    {
        tmr_outputs.push_back(FSMEvent(FSMEventCode::E_TMR_CV));
    }

    ///@TODO
#if 0 // FIXME
        // then those of sub-machine for phrase control
        flag, t = psm.timer.update()
        if flag)
            tmr_outputs.push_back(SMEvent(FSMEventCode::E_TMR_SR))
#endif
}


void FSMLoop::crank(const FSMEvent& this_event, tListEvent& state_outputs)
{
    state_outputs.clear();

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

                // new phrase state machine(idle, must be restarted)
                ///@TODO
                // FIXME psm = smphrase()

                // turn off any external action
                // announce halt
                state_outputs.push_back(
                    FSMEvent(FSMEventCode::E_XOFF));
                state_outputs.push_back(
                    FSMEvent(FSMEventCode::E_SAY, "session halted"));
                
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
                state_outputs.push_back(
                    FSMEvent(FSMEventCode::E_SAY, "get ready"));
            }
        }
    }
    else if (state == STATE_INH)
    {
        if (this_event.Code() == FSMEventCode::E_TMR_CV)
        {
            _to_norm();
            // announce start of monitoring
            state_outputs.push_back(
                FSMEvent(FSMEventCode::E_SAY, "go"));
        }
    }
    else if (state == STATE_NORM)
    {
        // in NORM pass event to phrase sub - machine
        ///@TODO
        // FIXME state_outputs.extend(psm.crank(this_event));

        if (this_event.Code() == FSMEventCode::E_CVOK)
        {
            _to_norm();
        }
        else if (this_event.Code() == FSMEventCode::E_TMR_CV)
        {
            cv_timer.start(FSMLoop::WARN_TIMEOUT_SEC);
            state = STATE_WARN;
        }
        else if (this_event.Code() == FSMEventCode::E_SRFAIL)
        {
            _to_act(state_outputs);
        }
    }
    else if (state == STATE_WARN)
    {
        // in WARN pass event to phrase sub - machine
        ///@TODO
        // FIXME state_outputs.extend(psm.crank(this_event));

        if (this_event.Code() == FSMEventCode::E_CVOK)
        {
            _to_norm();
        }
        else if (this_event.Code() == FSMEventCode::E_TMR_CV)
        {
            _to_act(state_outputs);
        }
        else if (this_event.Code() == FSMEventCode::E_SRFAIL)
        {
            _to_act(state_outputs);
        }
    }
    else if (state == STATE_ACT)
    {
        if (this_event.Code() == FSMEventCode::E_TMR_CV)
        {
            _to_norm();
            // restart phrase machine
            ///@TODO
// FIXME            state_outputs.extend(psm.crank(SMEvent(FSMEventCode::E_GO)))
            // turn off any external action
            state_outputs.push_back(FSMEvent(FSMEventCode::E_XOFF));
        }
    }
}

std::string FSMLoop::Snapshot(const std::string& rskey) const
{
    std::string result = "";
    if (snapshot.count(rskey))
    {
        result = snapshot.find(rskey)->second;
    }
    return result;
}
