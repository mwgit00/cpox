#ifndef POLLED_TIMER_H_
#define POLLED_TIMER_H_

#include <cstdint>
#include <chrono>

/*
A PolledTimer object must have its update() method applied in a
polling loop.  It's best to do this several times per second.
*/

#define MIN_INTERVAL    (1)
#define MAX_INTERVAL    (3600)

class PolledTimer
{
public:

    PolledTimer();
    virtual ~PolledTimer();

    void stop(void);
    uint32_t sec(void) const;
    void start(const uint32_t n_seconds);
    bool update(uint32_t& n_seconds_remaning);

private:
    
    std::chrono::time_point<std::chrono::steady_clock> tp_later;
    uint32_t _seconds_remaining;
    uint32_t _maxsec;
};

#endif // POLLED_TIMER_H_
