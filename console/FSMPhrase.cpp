#include "FSMPhrase.h"


FSMPhrase::FSMPhrase(tPhraseCfg& r) :
    rCfg(r),
    is_enabled(false),
    state(STATE_IDLE),
    strikes(0)
{
    snapshot.color = SCA_BLACK;
}


FSMPhrase::~FSMPhrase()
{
}


bool FSMPhrase::is_idle() const
{
    return (state == STATE_IDLE);
}


bool FSMPhrase::is_stopped() const
{
    return (state == STATE_STOP);
}


void FSMPhrase::_to_wait(void)
{
    // helper for transition to wait state (no outputs generated)
    sr_timer.start(rCfg.wait_time);
    state = STATE_WAIT;
}


void FSMPhrase::check_timers(tEventQueue& rq)
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
        rq.push(FSMEvent(FSMEventCode::E_TMR_SR));
    }
}


void FSMPhrase::crank(const FSMEvent& this_event, tEventQueue& rq)
{
    // CHECK FOR HIGH-PRIORITY STOP
    // if in any state other than idle and STOP occurs
    // - reset strikes
    // - stop timer
    // - enter STOP state
    // - send cancel to Speech Manager
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
        if (this_event.Code() == FSMEventCode::E_SR_RESTART)
        {
            // if listen-and-repeat mode is enabled then
            // enter wait state when commanded by main state machine
            if (is_enabled)
            {
                _to_wait();
            }
        }
    }
    else if (state == STATE_WAIT)
    {
        if (this_event.Code() == FSMEventCode::E_TMR_SR)
        {
            // new phrase to be spoken and repeated
            sr_timer.start(rCfg.spk_time);
            state = STATE_SPEAKING;
            rq.push(FSMEvent(FSMEventCode::E_SR_PHRASE));
        }
    }
    else if (state == STATE_SPEAKING)
    {
        if (this_event.Code() == FSMEventCode::E_TMR_SR)
        {
            // was speaking but timed out
            // likely due to Speech Manager not started (or hung)
            _to_wait();
        }
        else if (this_event.Code() == FSMEventCode::E_UDP_TTS_OK)
        {
            // begin boop (should get feedback before 1 second)
            sr_timer.start(1);
            state = STATE_BOOP;
            rq.push(FSMEvent(FSMEventCode::E_UDP_WAV, "c:\\work\\boop.wav"));
        }
    }
    else if (state == STATE_BOOP)
    {
        if (this_event.Code() == FSMEventCode::E_TMR_SR)
        {
            // was booping but timed out
            // likely due to Speech Manager not started (or hung)
            _to_wait();
        }
        else if (this_event.Code() == FSMEventCode::E_UDP_TTS_OK)
        {
            // begin recognition
            sr_timer.start(rCfg.rec_time);
            state = STATE_RECOGNIZING;
            rq.push(FSMEvent(FSMEventCode::E_SR_REC));
        }
    }
    else if (state == STATE_RECOGNIZING)
    {
        if (this_event.Code() == FSMEventCode::E_TMR_SR)
        {
            // was recognizing but timed out
            // likely due to Speech Manager not started (or hung)
            _to_wait();
        }
        else if (this_event.Code() == FSMEventCode::E_UDP_REC_VAL)
        {
            // got a result so update strike count
            // then ack result with the number of strikes
            _to_wait();

            // update strike count based on result code: 0-fail, 1-pass
            strikes = (this_event.Data() == 0) ? strikes + 1 : 0;

            rq.push(FSMEvent(FSMEventCode::E_SR_STRIKES, strikes));
            if (strikes == 3)
            {
                rq.push(FSMEvent(FSMEventCode::E_SR_FAIL));
            }
        }
    }
    else if (state == STATE_STOP)
    {
        if (this_event.Code() == FSMEventCode::E_SR_RESTART)
        {
            // resume phrase listen-and-repeat
            _to_wait();
        }
        else if (this_event.Code() == FSMEventCode::E_SR_RESET)
        {
            // loop will need to be restarted from GUI
            state = STATE_IDLE;
        }
    }
}

const FSMSnapShot& FSMPhrase::Snapshot() const
{
    return snapshot;
}
