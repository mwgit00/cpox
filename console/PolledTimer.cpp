#include "PolledTimer.h"

#include <algorithm>

PolledTimer::PolledTimer() :
    tp_later(),
    _seconds_remaining(0u),
    _maxsec(0u)
{
}

PolledTimer::~PolledTimer()
{
}

void PolledTimer::stop(void)
{
    // puts timer into stopped state
    _seconds_remaining = 0u;
}

uint32_t PolledTimer::sec(void) const
{
    // returns seconds remaining as of last update

    return _seconds_remaining;
}

void PolledTimer::start(const uint32_t n_seconds)
{
    // starts a timer
    // expiration time is relative to current steady time
    
    // apply allowed bounds on time interval
    uint32_t checked_seconds =
        std::min<uint32_t>(MAX_INTERVAL, std::max<uint32_t>(n_seconds, MIN_INTERVAL));

    // set expiration time in the future
    std::chrono::time_point<std::chrono::steady_clock> tp_now =
        std::chrono::high_resolution_clock::now();
    tp_later = tp_now + std::chrono::seconds(n_seconds);

    // set integer seconds remaining with input value
    _seconds_remaining = n_seconds;
    _maxsec = n_seconds;
}

bool PolledTimer::update(uint32_t& n_seconds_remaning)
{
    // services the timer
    // call this routine in a fast polling loop.
    // returns true if timer just expired, false otherwise
    // passes back time remaining rounded up to nearest second

    bool result = false;
    if (_seconds_remaining != 0u)
    {
        std::chrono::time_point<std::chrono::steady_clock> tp_now =
            std::chrono::high_resolution_clock::now();

        if (tp_now > tp_later)
        {
            // set seconds remaining to 0 to disable timer
            // issue "one-shot" flag for expiration
            _seconds_remaining = 0u;
            result = true;
        }
        else
        {
            // update seconds remaining
            _seconds_remaining = static_cast<int>(std::chrono::duration_cast<std::chrono::seconds>(tp_later - tp_now).count()) + 1;
        }
    }
    
    n_seconds_remaning = _seconds_remaining;
    return result;
}
