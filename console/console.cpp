// console.cpp : Defines the entry point for the console application.

#include <thread>
#include <sstream>
#include <iostream>

#include "AppMain.h"
#include "MTQueue.hpp"
#include "TTSTask.h"

#include "winsock2.h"
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Mswsock.lib")
#pragma comment(lib, "AdvApi32.lib")


//String haar_cascade_path = "C:\\opencv-3.2.0\\opencv\\build\\etc\\haarcascades\\";
//String path = "~/work/cpox/";
//String pathx = "~/work/cpox/movie/";

void thread_func(int k, int n, int t, MTQueue<std::string> * pq)
{
    // push a bunch of messages into queue at regular interval
    for (int i = 0; i < n; i++)
    {
        std::ostringstream oss;
        oss << "hey_" << k << "_" << i;
        pq->push(oss.str());
        std::this_thread::sleep_for(std::chrono::milliseconds(t));
    }
}

void drain_func(MTQueue<std::string> * pq)
{
    // drain queue at 10ms interval for 5 seconds
    int k = 0;
    while ((k++) < 500)
    {
        if (pq->size())
        {
            std::string s = pq->pop();
            std::cout << s << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void test_mtqueue()
{
    MTQueue<std::string> q;
    std::thread t1(thread_func, 1, 20, 7, &q);
    std::thread t2(thread_func, 2, 21, 11, &q);
    std::thread t3(thread_func, 3, 23, 13, &q);
    std::thread t4(drain_func, &q);
    t1.join();
    t2.join();
    t3.join();
    t4.join();
}


int main(int argc, char** argv)
{
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;

    // Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h
    wVersionRequested = MAKEWORD(2, 2);

    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0)
    {
        printf("WSAStartup failed with error: %d\n", err);
        return 1;
    }

    if ((LOBYTE(wsaData.wVersion) != 2) || (HIBYTE(wsaData.wVersion) != 2))
    {
        printf("Could not find a usable version of Winsock.dll\n");
        WSACleanup();
        return 1;
    }

#if 0
    msgq.push("hello world");
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    msgq.push("");
    test_mtqueue();
#endif
    
    AppMain app;
    app.Go();

    WSACleanup();
    return 0;
}
