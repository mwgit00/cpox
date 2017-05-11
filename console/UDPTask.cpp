
#include <stdio.h>

#include <winsock2.h>
#include <WS2tcpip.h>

#include "UDPTask.h"

#define BUFLEN  (256)


void udp_task_func(
    const std::string& rsIP,
    const uint16_t port_rx,
    const uint16_t port_tx,
    tEventQueue& rqrx,
    tEventQueue& rqtx)
{
    bool result = true;

    SOCKET the_socket = INVALID_SOCKET;
    struct sockaddr_in server;
    struct sockaddr_in si_other;
    int err;
    int slen;
    int recv_len;
    char buf[BUFLEN];

    std::wstring ws(rsIP.begin(), rsIP.end());

    // init address and port for outgoing packets
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(port_tx);
    InetPton(AF_INET, ws.c_str(), &si_other.sin_addr.s_addr);

    // clear UDP RX buffer
    memset(buf, '\0', BUFLEN);

    // create UDP datagram socket
    the_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (the_socket == INVALID_SOCKET)
    {
        result = false;
    }

    // re-use address in hopes that it will prevent timed wait state
    if (result)
    {
        DWORD optval = 1;
        err = setsockopt(the_socket, SOL_SOCKET, SO_REUSEADDR, (const char *)&optval, sizeof(optval));
        if (err == SOCKET_ERROR)
        {
            result = false;
        }
    }

    // set receive timeout to 100ms for sleepy loop
    if (result)
    {
        DWORD tv = 100;
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

    if (result)
    {
        // report successful initialization
        rqtx.push(FSMEvent(FSMEventCode::E_UDP_UP, 1));
    }

    while (result)
    {
        // receive, blocks until timeout
        slen = (int) sizeof(struct sockaddr_in);
        recv_len = recvfrom(the_socket, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen);
        
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
                err = sendto(the_socket, s.data(), n, 0, (struct sockaddr*) &si_other, slen);
                if (err == SOCKET_ERROR)
                {
                    result = false;
                }
            }
        }
    }

    closesocket(the_socket);

    // report thread termination
    rqtx.push(FSMEvent(FSMEventCode::E_UDP_UP, 0));
}
