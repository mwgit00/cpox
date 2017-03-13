

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


static void tts_thread_func(TTSTask * ptask)
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
            tEventQueue& rq = ptask->GetRxQueue();
            if (rq.size())
            {
                FSMEvent x = rq.pop();
                if (x.Code() == FSMEventCode::E_TASK_HALT)
                {
                    break;
                }
                
                const std::string& smsg = x.Str();
                std::wstring wsmsg(smsg.begin(), smsg.end());

                // this blocks until it is done
                hr = pVoice->Speak(wsmsg.c_str(), 0, NULL);
                ptask->GetTxQueue().push(FSMEvent(FSMEventCode::E_TTS_IDLE));
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
    ptask->SetDone();
}


TTSTask::TTSTask() :
    is_thread_done(false),
    pqtx(nullptr)
{
}


TTSTask::~TTSTask()
{
}


void TTSTask::SetDone(void)
{
    is_thread_done = true;
}


bool TTSTask::GetDone(void)
{
    return is_thread_done;
}


tEventQueue& TTSTask::GetTxQueue(void)
{
    return *pqtx;
}


tEventQueue& TTSTask::GetRxQueue(void)
{
    return qrx;
}


void TTSTask::assign_tx_queue(tEventQueue * p)
{
    pqtx = p;
}


void TTSTask::post_event(const FSMEvent& x)
{
    qrx.push(x);
}


void TTSTask::go(void)
{
    std::thread tx(tts_thread_func, this);
    tx.detach();
}


void TTSTask::stop(void)
{
    // command helper tasks to halt
    qrx.push(FSMEvent(FSMEventCode::E_TASK_HALT));
    while (!is_thread_done)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}
