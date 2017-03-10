#ifndef TTS_TASK_H_
#define TTS_TASK_H_

#include "defs.h"

class TTSTask
{
public:

    const std::string name = "TTS";
    
    TTSTask();
    virtual ~TTSTask();

    void assign_msg_queue(tMsgQueue * p);
    void go(void);

private:
    
    tMsgQueue * pmsgq;
};

#endif // TTS_TASK_H_
