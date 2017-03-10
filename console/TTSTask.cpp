

// the following statements come straight from the SAPI tutorial

#include <Windows.h>
#define _ATL_APARTMENT_THREADED
#include <atlbase.h>
//You may derive a class from CComModule and use it if you want to override something,
//but do not change the name of _Module
extern CComModule _Module;
#include <atlcom.h>
#include <sapi.h>


#include "TTSTask.h"


static void tts_thread_func(tMsgQueue * pq)
{
    bool result = true;
    ISpVoice * pVoice = NULL;
    HRESULT hr;

    if (FAILED(::CoInitialize(NULL)))
    {
        result = false;
    }

    if (result)
    {
        hr = CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_ALL, IID_ISpVoice, (void **)&pVoice);
        if (SUCCEEDED(hr))
        {
            // report success
        }
        else
        {
            result = false;
        }
    }

    if (result)
    {
        while (true)
        {
            if (pq->size())
            {
                std::string smsg(pq->pop());
                if (smsg == "")
                {
                    break;
                }
                
                std::wstring wsmsg(smsg.begin(), smsg.end());
                hr = pVoice->Speak(wsmsg.c_str(), 0, NULL);
            }
            else
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }

        pVoice->Release();
        pVoice = NULL;
    }

    ::CoUninitialize();
}


TTSTask::TTSTask()
{
}


TTSTask::~TTSTask()
{
}


void TTSTask::assign_msg_queue(tMsgQueue * p)
{
    pmsgq = p;
}


void TTSTask::go(void)
{
    std::thread tx(tts_thread_func, pmsgq);
    tx.detach();
}
