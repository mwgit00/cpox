#include "Windows.h"

#include <string>
#include "COMTask.h"


enum
{
    STATE_IDLE,
    STATE_RESP,
    STATE_CMD,
    STATE_PARAM,
};

int com_state = STATE_IDLE;


// returns a 1 if valid response found
// response is of form (r[0-3][a-y])

void com_crank(char c, int& smval)
{
    if (com_state == STATE_IDLE)
    {
        if (c == 'r')
        {
            com_state = STATE_RESP;
        }
    }
    else if (com_state == STATE_RESP)
    {
        if (c >= '0' || c <= '3')
        {
            com_state = STATE_CMD;
        }
        else if (c == 'r')
        {
            // wonky protocol
            // unexpected 'r' so it may be start of new response
            com_state = STATE_RESP;
        }
        else
        {
            com_state = STATE_IDLE;
        }
    }
    else if (com_state == STATE_CMD)
    {
        if (c >= 'a' || c <= 'y')
        {
            // valid response
            // in this case, the 'r' is not the start of a response
            smval = 1;
        }
        com_state = STATE_IDLE;
    }
}


void com_task_func(tEventQueue& rqrx, tEventQueue& rqtx)
{
    bool result = true;

    HANDLE hComm;
    DCB dcb = { 0 };
    COMMTIMEOUTS timeouts;

    // for RX, returns immediately
    // no timeouts for TX
    timeouts.ReadIntervalTimeout = MAXDWORD;
    timeouts.ReadTotalTimeoutMultiplier = 0;
    timeouts.ReadTotalTimeoutConstant = 0;
    timeouts.WriteTotalTimeoutMultiplier = 0;
    timeouts.WriteTotalTimeoutConstant = 0;

    hComm = CreateFile(L"COM2",
        GENERIC_READ | GENERIC_WRITE,
        0,
        0,
        OPEN_EXISTING,
        0,
        0);
    
    if (hComm == INVALID_HANDLE_VALUE)
    {
        // error opening port; abort
        result = false;
    }

    if (result)
    {
        result = false;

        // update comm settings
        FillMemory(&dcb, sizeof(dcb), 0);
        dcb.BaudRate = CBR_2400;
        dcb.Parity = NOPARITY;
        dcb.ByteSize = DATABITS_8;
        dcb.StopBits = ONESTOPBIT;

        // Set new state.
        if (SetCommState(hComm, &dcb))
        {
            // if here then port is good to go
            result = true;
        }
    }

    if (result)
    {
        if (!SetCommTimeouts(hComm, &timeouts))
        {
            // Error setting time-outs.
            result = false;
        }
    }

    if (result)
    {
        // report successful initialization
        rqtx.push(FSMEvent(FSMEventCode::E_COM_UP, 1));
    }

    // if all is well then enter a sleepy loop
    // that handles requests to serial RX and TX
    
    if (result)
    {
        char buff[64];
        bool is_running = true;
        int k = 0;
        
        while (is_running)
        {
            // read bytes from the COM port
            // returns immediately
            DWORD nread = 0;
            BOOL rx_result = ReadFile(hComm, buff, sizeof(buff), &nread, NULL);
            if (rx_result)
            {
                if (nread)
                {
                    for (DWORD i = 0; i < nread; i++)
                    {
                        int smval = 0;
                        com_crank(buff[i], smval);
                        if (smval)
                        {
                            rqtx.push(FSMEvent(FSMEventCode::E_COM_ACK, std::string(buff, 3)));
                        }
                    }
                }
            }
            
            while (rqrx.size())
            {
                // end thread loop if commanded to halt
                FSMEvent x = rqrx.pop();
                if (x.Code() == FSMEventCode::E_TASK_HALT)
                {
                    is_running = false;
                    break;
                }
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(10));

            // periodic ping when idle
            k++;
            if (k == 100)
            {
                k = 0;
                DWORD nwritten;
                buff[0] = 'x';
                buff[1] = '0';
                buff[2] = 'a';
                BOOL tx_result = WriteFile(hComm, buff, 3, &nwritten, NULL);
                if (!tx_result)
                {
                    is_running = false;
                }
            }
        }

        // clean up
    }

    CloseHandle(hComm);
}
