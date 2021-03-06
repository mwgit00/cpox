#ifndef FSM_LOOP_H_
#define FSM_LOOP_H_

#include "defs.h"
#include "defs_cfg.h"
#include "FSMEvent.h"
#include "FSMSnapShot.h"
#include "PolledTimer.h"
#include "FSMPhrase.h"

#define FSM_FLAG_EYES   (0x1u)
#define FSM_FLAG_SMILE  (0x2u)
#define FSM_FLAG_LISTEN (0x4u)


class FSMLoop
{
public:

    // states
    enum
    {
        STATE_IDLE = 0, ///< stopped
        STATE_INH = 1,  ///< start - up delay
        STATE_NORM = 2, ///< valid face / eye detections
        STATE_WARN = 3, ///< string of misses, warning to user
        STATE_ACT = 4,  ///< act after too many misses
    };

    FSMLoop() = delete;
    FSMLoop(tLoopCfg& r);
    virtual ~FSMLoop();

    bool is_idle() const;
    void check_timers(tEventQueue& rq);
    void crank(const FSMEvent& this_event, tEventQueue& rq);
    const FSMSnapShot& Snapshot() const;

private:

    void _to_norm();
    void _to_act(tEventQueue& rq);

private:

    tLoopCfg& rCfg;

    int state;
    int external_output_level;  ///< zero-based output level
    PolledTimer cv_timer;
    FSMSnapShot snapshot;

    uint32_t flags;
};

#endif // FSM_LOOP_H_
