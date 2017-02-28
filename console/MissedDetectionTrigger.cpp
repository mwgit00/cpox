#include "MissedDetectionTrigger.h"

#include <ctime>
#include <chrono>

MissedDetectionTrigger::MissedDetectionTrigger() :
    state(STATE_IDLE),
    detect_timeout_ms(15000.0),
    inhibit_timeout_ms(30000.0),
    is_triggered(false),
    is_detecting(false),
    tp_update(),
    tp_detect_start(),
    tp_inhibit_start()
{
    // will start in a disabled state
    // update will have no effect until reset is called
}

MissedDetectionTrigger::~MissedDetectionTrigger()
{
}

bool MissedDetectionTrigger::isTriggered(void) const
{
    return is_triggered;
}

bool MissedDetectionTrigger::isDetecting(void) const
{
    return is_detecting;
}

double MissedDetectionTrigger::getDetectMsecRemaining(void) const
{
    double r = 0.0;
    if (state == STATE_DETECT)
    {
        std::chrono::time_point<std::chrono::steady_clock> tp =
            std::chrono::high_resolution_clock::now();
        r = detect_timeout_ms - std::chrono::duration<double, std::milli>(tp - tp_detect_start).count();
        if (r < 0.0)
        {
            r = 0.0;
        }
    }
    return r;
}

int MissedDetectionTrigger::getInhibitSecondsRemaining(void) const
{
    int n = 0;
    if (state == STATE_INHIBIT)
    {
        double r = 0.0;
        std::chrono::time_point<std::chrono::steady_clock> tp =
            std::chrono::high_resolution_clock::now();

        r = std::chrono::duration<double, std::milli>(tp - tp_inhibit_start).count();
        r = (inhibit_timeout_ms - r) / 1000.0;
        n = static_cast<int>(r) + 1;

        if (r < 0.0)
        {
            n = 0;
        }
    }
    return n;
}

void MissedDetectionTrigger::reset()
{
    state = STATE_IDLE;
    is_triggered = false;
    is_detecting = false;
}

void MissedDetectionTrigger::start()
{
    // start in INHIBIT state and clear one-shot flags
    is_triggered = false;
    is_detecting = false;
    state = STATE_INHIBIT;
    tp_inhibit_start = std::chrono::high_resolution_clock::now();
}

void MissedDetectionTrigger::set(const double t_miss_ms, const double t_inh_ms)
{
    // assign new timeout values
    detect_timeout_ms = t_miss_ms;
    inhibit_timeout_ms = t_inh_ms;
}

bool MissedDetectionTrigger::update(bool is_ok)
{
    // clear the one-shot flags
    is_triggered = false;
    is_detecting = false;
    
    // time stamp for this update
    tp_update = std::chrono::high_resolution_clock::now();
    
    if (state == STATE_INHIBIT)
    {
        double elapsed_ms = std::chrono::duration<double, std::milli>(tp_update - tp_inhibit_start).count();
        if (elapsed_ms > inhibit_timeout_ms)
        {
            // inhibit has timed out so enter detect state
            is_detecting = true;
            state = STATE_DETECT;
            tp_detect_start = tp_update;
        }
    }
    else if (state == STATE_DETECT)
    {
        if (is_ok)
        {
            // OK so reset start time
            tp_detect_start = std::chrono::high_resolution_clock::now();
        }
        else
        {
            // not OK so check elapsed time
            double elapsed_ms = std::chrono::duration<double, std::milli>(tp_update - tp_detect_start).count();
            if (elapsed_ms > detect_timeout_ms)
            {
                // too much time has passed
                // set triggered flag and go back to inhibit state
                is_triggered = true;
                state = STATE_INHIBIT;
                tp_inhibit_start = std::chrono::high_resolution_clock::now();
            }
        }
    }

    return true;
}
