#ifndef FSM_PHRASE_H_
#define FSM_PHRASE_H_

#include "defs.h"
#include "defs_cfg.h"
#include "FSMEvent.h"
#include "FSMSnapShot.h"
#include "PolledTimer.h"

class FSMPhrase
{
public:

    enum
    {
        STATE_IDLE = 0,
        STATE_WAIT = 1,
        STATE_SPEAKING = 2,
        STATE_RECOGNIZING = 3,
        STATE_STOP = 4,
    };

    FSMPhrase() = delete;
    FSMPhrase(tPhraseCfg& r);
    virtual ~FSMPhrase();

    void set_enabled(const bool f) { is_enabled = f; }

    bool is_idle() const;
    bool is_stopped() const;

    void check_timers(tEventQueue& rq);
    void crank(const FSMEvent& this_event, tEventQueue& rq);
    const FSMSnapShot& Snapshot() const;

private:

    void _to_wait(void);

private:

    tPhraseCfg& rCfg;
    
    bool is_enabled;
    int state;
    int strikes;
    PolledTimer sr_timer;
    FSMSnapShot snapshot;
};

#endif // FSM_PHRASE_H_
