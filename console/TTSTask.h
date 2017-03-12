#ifndef TTS_TASK_H_
#define TTS_TASK_H_

#include <atomic>
#include "FSMEvent.h"

class TTSTask
{
public:

    TTSTask();
    virtual ~TTSTask();

    tEventQueue& GetRxQueue(void);
    tEventQueue& GetTxQueue(void);

    void SetDone(void);
    bool GetDone(void);
    
    void assign_tx_queue(tEventQueue * p);
    void post_event(const FSMEvent& x);
    void go(void);
    void stop(void);

private:
    
    std::atomic_bool is_thread_done;
    
    tEventQueue qrx;    ///< for incoming (received) events
    tEventQueue * pqtx; ///< for outgoing (transmitted) events
};

#endif // TTS_TASK_H_
