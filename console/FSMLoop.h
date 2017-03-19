#ifndef FSM_LOOP_H_
#define FSM_LOOP_H_

#include "defs.h"
#include "FSMEvent.h"
#include "FSMSnapShot.h"
#include "PolledTimer.h"
#include "FSMPhrase.h"

class FSMLoop
{
public:

    // states
    enum
    {
        STATE_IDLE = 0, ///< stopped
        STATE_INH = 1, ///< start - up delay
        STATE_NORM = 2, ///< valid face / eye detections
        STATE_WARN = 3, ///< string of misses, warning to user
        STATE_ACT = 4, ///< act after too many misses
    };

    // timer settings
    enum
    {
        INH_TIMEOUT_SEC = 5, ///< delay before starting
        NORM_TIMEOUT_SEC = 4, ///< no face / eye in this time, goes to WARN
        WARN_TIMEOUT_SEC = 3, ///< no face / eye in this time, goes to ACT
        ACT_TIMEOUT_SEC = 5, ///< duration of ACT
    };

    FSMLoop();
    virtual ~FSMLoop();

    bool is_idle() const;
    void check_timers(tEventQueue& rq);
    void crank(const FSMEvent& this_event, tEventQueue& rq);
    const FSMSnapShot& Snapshot() const;

private:

    void _to_norm();
    void _to_act(tEventQueue& rq);

private:

    uint32_t state;
    PolledTimer cv_timer;
    FSMSnapShot snapshot;

    uint32_t level;
};

#endif // FSM_LOOP_H_
