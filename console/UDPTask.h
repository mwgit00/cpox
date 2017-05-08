#ifndef UDP_TASK_H_
#define UDP_TASK_H_

#include "FSMEvent.h"

void udp_task_func(
    const std::string& rsIP,
    const uint16_t port_rx,
    const uint16_t port_tx,
    tEventQueue& rqrx,
    tEventQueue& rqtx);

#endif // UDP_TASK_H_
