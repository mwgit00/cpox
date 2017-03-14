#include "FSMPhrase.h"


FSMPhrase::FSMPhrase() :
    state(STATE_STOP),
    sr_timer(),
    strikes(0u)
{
    snapshot.color = SCA_BLACK;
}


FSMPhrase::~FSMPhrase()
{
}


void FSMPhrase::_to_wait(void)
{
    // helper for transition to wait state (no outputs generated)
    sr_timer.start(WAIT_TIMEOUT_SEC);
    state = STATE_WAIT;
}


void FSMPhrase::check_timers(tListEvent& tmr_outputs)
{
    // update the snapshot FIRST
    // then update the timer

    snapshot.state = state;
    snapshot.sec = sr_timer.sec();
    snapshot.prog = (state == STATE_RECOGNIZING) ? snapshot.sec : 0;
    if ((state == STATE_IDLE) || (state == STATE_STOP))
    {
        snapshot.color = SCA_BLACK;
    }
    else
    {
        snapshot.color = ((strikes) ? SCA_YELLOW_MED : SCA_GREEN_MED);
    }
    
    uint32_t sec;
    bool flag = sr_timer.update(sec);
    if (flag)
    {
        tmr_outputs.push_back(FSMEvent(FSMEventCode::E_TMR_SR));
    }
}


void FSMPhrase::crank(const FSMEvent& this_event, tListEvent& state_outputs)
{
    // CHECK FOR HIGH-PRIORITY STOP
    // if in any state other than idle and STOP occurs
    // - reset strikes
    // - stop timer
    // - enter STOP state
    if (state != STATE_IDLE)
    {
        if (this_event.Code() == FSMEventCode::E_SR_STOP)
        {
            strikes = 0;
            sr_timer.stop();
            state = STATE_STOP;
            ///////
            return;
            ///////
        }
    }

    if (state == STATE_IDLE)
    {
        // if key LISTEN then TO WAIT
        if (this_event.Code() == FSMEventCode::E_KEY)
        {
            if (this_event.Data() == KEY_LISTEN)
            {
                // announce start of speech mode
                _to_wait();
                state_outputs.push_back(FSMEvent(FSMEventCode::E_TTS_SAY,
                    "listen and repeat"));
            }
        }
    }
    else if (state == STATE_WAIT)
    {
        if (this_event.Code() == FSMEventCode::E_TMR_SR)
        {
            // new phrase to be spoken and repeated
            sr_timer.start(SPK_TIMEOUT_SEC);
            state = STATE_SPEAKING;
            state_outputs.push_back(FSMEvent(FSMEventCode::E_SR_PHRASE));
        }
    }
    else if (state == STATE_SPEAKING)
    {
        if (this_event.Code() == FSMEventCode::E_TMR_SR)
        {
            // was speaking but timed out
            // likely due to TTS process not started
            _to_wait();
        }
        else if (this_event.Code() == FSMEventCode::E_TTS_IDLE)
        {
            // begin recognition
            sr_timer.start(REC_TIMEOUT_SEC);
            state = STATE_RECOGNIZING;
            state_outputs.push_back(FSMEvent(FSMEventCode::E_SR_REC));
        }
    }
    else if (state == STATE_RECOGNIZING)
    {
        if (this_event.Code() == FSMEventCode::E_TMR_SR)
        {
            // was recognizing but timed out
            // likely due to REC process not started (or hung)
            _to_wait();
        }
        else if (this_event.Code() == FSMEventCode::E_SR_RESULT)
        {
            // got a result so update strike count
            // then ack result with the number of strikes
            _to_wait();
            uint32_t ack_strikes = 0u;

            // 0-fail 1-pass
            if (this_event.Data() == 0)
            {
                strikes++;
                ack_strikes = strikes;
            }
            else
            {
                strikes = 0;
            }

            state_outputs.push_back(FSMEvent(FSMEventCode::E_SR_STRIKES, ack_strikes));
        }
    }
    else if (state == STATE_STOP)
    {
        if (this_event.Code() == FSMEventCode::E_SR_RESTART)
        {
            // this restarts the phrase listen-and-repeat operation
            // after an external action has been triggered
            _to_wait();
        }
        else if (this_event.Code() == FSMEventCode::E_SR_RESET)
        {
            state = STATE_IDLE;
        }
    }
}

const FSMSnapShot& FSMPhrase::Snapshot() const
{
    return snapshot;
}
