#ifndef COM_TASK_H_
#define COM_TASK_H_

#include "FSMEvent.h"

typedef enum : char
{
    CMD_SYNC = 'C',         // start of cmd recognized by ext device
    CMD_RESP = 'R',         // start of response to cmd
    CMD_PING = '0',         // ping, response has max of level settings
    CMD_LEVEL_1 = '1',      // set level 1 to value a-y (25 settings)
    CMD_LEVEL_2 = '2',      // set level 2 to value a-y (25 settings)
    CMD_OUTPUT_ON = '3',    // activate outputs
    CMD_VAL_MIN = 'a',      // minimum value setting
    CMD_VAL_MAX = 'y',      // maximum value setting
} eExtCtrl;

void com_task_func(
    const std::string& rsPortName,
    tEventQueue& rqrx,
    tEventQueue& rqtx);

#endif // COM_TASK_H_
