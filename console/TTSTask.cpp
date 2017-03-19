

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


void tts_task_func(tEventQueue& rqrx, tEventQueue& rqtx)
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
            // report successful initialization
            rqtx.push(FSMEvent(FSMEventCode::E_TTS_UP, 1));
        }
        else
        {
            result = false;
        }
    }

    // if all is well then enter a sleepy loop
    // that handles requests to say phrases
    if (result)
    {
        while (true)
        {
            if (rqrx.size())
            {
                // end thread loop if commanded to halt
                FSMEvent x = rqrx.pop();
                if (x.Code() == FSMEventCode::E_TASK_HALT)
                {
                    break;
                }
                
                // convert to "wide" string for the speech API
                const std::string& smsg = x.Str();
                std::wstring wsmsg(smsg.begin(), smsg.end());

                // say phrase which blocks until it is done
                // report when phrase is completed
                hr = pVoice->Speak(wsmsg.c_str(), 0, NULL);
                rqtx.push(FSMEvent(FSMEventCode::E_TTS_IDLE));
            }
            else
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }

        // clean up
        pVoice->Release();
        pVoice = NULL;
    }

    ::CoUninitialize();

    // report thread termination
    rqtx.push(FSMEvent(FSMEventCode::E_TTS_UP, 0));
}
