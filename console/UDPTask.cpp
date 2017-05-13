
#include <stdio.h>

#include <winsock2.h>
#include <WS2tcpip.h>

#include "UDPTask.h"

#define BUFLEN  (256)


void thr_func_rx(UDPTask& r)
{
    r.rx_loop();
}


void thr_func_tx(UDPTask& r)
{
    r.tx_loop();
}


UDPTask::UDPTask(tEventQueue& rqrx, tEventQueue& rqtx) :
    rqrx(rqrx),
    rqtx(rqtx),
    the_socket(INVALID_SOCKET),
    is_socket_ok(false)
{
    atomic_is_stop_requested.store(false);
}


UDPTask::~UDPTask(void)
{

}


void UDPTask::configure(
    const std::string& rsIP,
    const uint16_t port_rx,
    const uint16_t port_tx)
{
    bool result = true;
    this->rsIP = rsIP;
    this->port_rx = port_rx;
    this->port_tx = port_tx;

    struct sockaddr_in server;
    int err;

    // create UDP datagram socket
    the_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (the_socket == INVALID_SOCKET)
    {
        result = false;
    }

    // set option to re-use address
    // this may not be necessary but I dont think it's hurting anything
    if (result)
    {
        DWORD optval = 1;
        err = setsockopt(the_socket, SOL_SOCKET, SO_REUSEADDR, (const char *)&optval, sizeof(optval));
        if (err == SOCKET_ERROR)
        {
            result = false;
        }
    }

    // set receive timeout to 100ms for sleepy RX loop
    if (result)
    {
        DWORD tv = 500;
        err = setsockopt(the_socket, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv, sizeof(tv));
        if (err == SOCKET_ERROR)
        {
            result = false;
        }
    }

    if (result)
    {
        server.sin_family = AF_INET;
        server.sin_addr.s_addr = INADDR_ANY;
        server.sin_port = htons(port_rx);
        err = bind(the_socket, (struct sockaddr *)&server, sizeof(server));
        if (err == SOCKET_ERROR)
        {
            result = false;
        }
    }

    // threads won't go unless this is true
    is_socket_ok = result;
}


void UDPTask::Go()
{
    thr_rx = std::thread(thr_func_rx, std::ref(*this));
    thr_tx = std::thread(thr_func_tx, std::ref(*this));
}


void UDPTask::Stop()
{
    atomic_is_stop_requested.store(true);
    
    if (thr_rx.joinable())
    {
        thr_rx.join();
    }

    if (thr_tx.joinable())
    {
        thr_tx.join();
    }

    shutdown(the_socket, SD_BOTH);
    closesocket(the_socket);
}


void UDPTask::rx_loop(void)
{
    bool result = is_socket_ok;
    
    int err;
    int recv_len;
    char buf[BUFLEN];

    if (result)
    {
        // report successful RX initialization
        rqtx.push(FSMEvent(FSMEventCode::E_UDP_UP, UDP_RX_MASK | 1));
    }

    while (!atomic_is_stop_requested.load() && result)
    {
        // receive, blocks until timeout
        recv_len = recv(the_socket, buf, BUFLEN, 0);

        if (recv_len == SOCKET_ERROR)
        {
            err = WSAGetLastError();
            if (err == WSAETIMEDOUT)
            {
                // timeout is ok
            }
            else
            {
                result = true;
            }
        }
        else if (recv_len > 0)
        {
            std::string s(buf, recv_len);
            if (s == "tts 1")
            {
                rqtx.push(FSMEvent(FSMEventCode::E_UDP_TTS_OK));
            }
            else if (s == "rec 0")
            {
                rqtx.push(FSMEvent(FSMEventCode::E_UDP_REC_VAL, 0));
            }
            else if (s == "rec 1")
            {
                rqtx.push(FSMEvent(FSMEventCode::E_UDP_REC_VAL, 1));
            }
        }
    }

    // report abnormal RX thread termination
    rqtx.push(FSMEvent(FSMEventCode::E_UDP_UP, UDP_RX_MASK | 0));
}


void UDPTask::tx_loop(void)
{
    bool result = is_socket_ok;

    // init address and port for outgoing packets
    struct sockaddr_in si_other;
    std::wstring ws(rsIP.begin(), rsIP.end());
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(port_tx);
    InetPton(AF_INET, ws.c_str(), &si_other.sin_addr.s_addr);
    
    if (result)
    {
        // report successful TX initialization
        rqtx.push(FSMEvent(FSMEventCode::E_UDP_UP, UDP_TX_MASK | 1));
    }

    while (!atomic_is_stop_requested.load() && result)
    {
        while (rqrx.size() && result)
        {
            std::string s;
            FSMEvent x = rqrx.pop();
            switch (x.Code())
            {
                case FSMEventCode::E_TASK_HALT:
                {
                    // end thread loop if commanded to halt
                    result = false;
                    break;
                }
                case FSMEventCode::E_UDP_SAY:
                {
                    s = "say " + x.Str();
                    break;
                }
                case FSMEventCode::E_UDP_LOAD:
                {
                    s = "load " + x.Str();
                    break;
                }
                case FSMEventCode::E_UDP_REPEAT:
                {
                    s = "repeat";
                    break;
                }
                case FSMEventCode::E_UDP_REC:
                {
                    s = "rec";
                    break;
                }
                default:
                {
                    break;
                }
            }

            if (s.length())
            {
                // send command to speech manager server
                int n = static_cast<int>(s.length());
                int slen = (int) sizeof(struct sockaddr_in);
                int err = sendto(the_socket, s.data(), n, 0, (struct sockaddr*) &si_other, slen);
                if (err == SOCKET_ERROR)
                {
                    result = false;
                }
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(60));
    }

    // report abnormal TX thread termination
    rqtx.push(FSMEvent(FSMEventCode::E_UDP_UP, UDP_TX_MASK | 0));
}
