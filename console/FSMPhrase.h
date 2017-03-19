#ifndef FSM_PHRASE_H_
#define FSM_PHRASE_H_

#include "defs.h"
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

    enum
    {
        WAIT_TIMEOUT_SEC = 10, ///< interval between end of rec and start of spk
        REC_TIMEOUT_SEC = 25, ///< at least 10sec longer than poxrec.py timeout
        SPK_TIMEOUT_SEC = 10, ///< should be longer than longest phrase
    };

    FSMPhrase();
    virtual ~FSMPhrase();

    void check_timers(tEventQueue& rq);
    void crank(const FSMEvent& this_event, tEventQueue& rq);
    const FSMSnapShot& Snapshot() const;

private:

    void _to_wait(void);

private:

    uint32_t state;
    PolledTimer sr_timer;
    FSMSnapShot snapshot;

    uint32_t strikes;
};

#endif // FSM_PHRASE_H_
