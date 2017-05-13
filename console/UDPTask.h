#ifndef UDP_TASK_H_
#define UDP_TASK_H_

#include <thread>
#include <atomic>
#include <string>
#include <winsock2.h>
#include "FSMEvent.h"

#define UDP_UP_MASK (0x01u)
#define UDP_RX_MASK (0x02u)
#define UDP_TX_MASK (0x04u)

class UDPTask
{
public:
    
    UDPTask(void) = delete;
    UDPTask(tEventQueue& rqrx, tEventQueue& rqtx);
    virtual ~UDPTask(void);
    
    void configure(
        const std::string& rsIP,
        const uint16_t port_rx,
        const uint16_t port_tx);

    void Go();
    void Stop();

    void rx_loop(void);
    void tx_loop(void);

private:

    tEventQueue& rqrx;
    tEventQueue& rqtx;

    std::thread thr_rx;
    std::thread thr_tx;

    std::string rsIP;
    uint16_t port_rx;
    uint16_t port_tx;

    SOCKET the_socket;
    bool is_socket_ok;
    std::atomic<bool> atomic_is_stop_requested;
};

#endif // UDP_TASK_H_
