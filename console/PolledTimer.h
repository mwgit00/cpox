#ifndef POLLED_TIMER_H_
#define POLLED_TIMER_H_

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
    int sec(void) const;
    void start(const int n_seconds);
    bool update(int& n_seconds_remaning);

private:
    
    std::chrono::time_point<std::chrono::steady_clock> tp_later;
    int _seconds_remaining;
    int _maxsec;
};

#endif // POLLED_TIMER_H_
