#ifndef COM_TASK_H_
#define COM_TASK_H_

#include "FSMEvent.h"

void com_task_func(
    const std::string& rsPortName,
    tEventQueue& rqrx,
    tEventQueue& rqtx);

#endif // COM_TASK_H_
