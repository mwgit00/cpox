#include "Windows.h"

#include <string>
#include "COMTask.h"


#define LOOP_DELAY_MS           (10)
#define LOOP_CT_1SEC            (1000 / LOOP_DELAY_MS)
#define LOOP_CT_SELF_Q_DRAIN    (100 / LOOP_DELAY_MS)
#define LOOP_CT_LEVEL_ADJ       (15)

enum
{
    STATE_IDLE,
    STATE_RESP,
    STATE_CMD,
    STATE_PARAM,
};

int com_state = STATE_IDLE;
std::string s_ack = "???";
tEventQueue self_q;


bool send_cmd(HANDLE hComm, const char c_cmd, const char c_param)
{
    char buff[8];
    DWORD nwritten;
    buff[0] = 'x';
    buff[1] = c_cmd;
    buff[2] = c_param;
    BOOL tx_result = WriteFile(hComm, buff, 3, &nwritten, NULL);
    return (tx_result) ? true : false;
}


// returns a 1 if valid response found
// response is of form (r[0-3][a-y])

void com_crank(char c, int& smval)
{
    if (com_state == STATE_IDLE)
    {
        if (c == 'r')
        {
            s_ack[0] = c;
            com_state = STATE_RESP;
        }
    }
    else if (com_state == STATE_RESP)
    {
        if (c >= '0' || c <= '3')
        {
            s_ack[1] = c;
            com_state = STATE_CMD;
        }
        else if (c == 'r')
        {
            // wonky protocol
            // unexpected 'r' so it may be start of new response
            s_ack[0] = c;
            com_state = STATE_RESP;
        }
        else
        {
            com_state = STATE_IDLE;
        }
    }
    else if (com_state == STATE_CMD)
    {
        if ((c >= 'a') && (c <= 'y'))
        {
            // valid parameter in response is a-y
            // in this case, the 'r' is not the start of a response
            s_ack[2] = c;
            smval = 1;
        }
        com_state = STATE_IDLE;
    }
}


void com_task_func(tEventQueue& rqrx, tEventQueue& rqtx)
{
    bool result = true;

    HANDLE hComm = INVALID_HANDLE_VALUE;
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
        int k_time_div = 0;
        
        while (is_running)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(LOOP_DELAY_MS));

            // read bytes from the COM port
            // call returns immediately
            // then check result and see if any bytes were read
            
            DWORD nread = 0;
            BOOL rx_result = ReadFile(hComm, buff, sizeof(buff), &nread, NULL);
            if (rx_result)
            {
                if (nread)
                {
                    // run state machine on received byte(s) to process ACKs
                    for (DWORD i = 0; i < nread; i++)
                    {
                        int smval = 0;
                        com_crank(buff[i], smval);
                        if (smval)
                        {
                            rqtx.push(FSMEvent(FSMEventCode::E_COM_ACK, s_ack));
                        }
                    }
                }
            }
            
            while (rqrx.size())
            {
                FSMEvent x = rqrx.pop();
                switch (x.Code())
                {
                    case FSMEventCode::E_TASK_HALT:
                    {
                        // end thread loop if commanded to halt
                        is_running = false;
                        break;
                    }
                    case FSMEventCode::E_COM_XON:
                    {
                        // command from app includes duration (count)
                        // queue up a number of ON commands for external device
                        // repeated commands must be sent for device to remain on

                        uint32_t ct = x.Data();
                        ct = (ct > 10) ? 10 : ct;
                        for (uint32_t i = 0; i < ct; i++)
                        {
                            self_q.push(FSMEvent(FSMEventCode::E_COM_XON, 1));
                        }
                        k_time_div = 0; 
                        break;
                    }
                    case FSMEventCode::E_COM_LEVEL:
                    {
                        // queue up fixed number of LEVEL commands for external device
                        // each output will need its own commands queued for it
                        // repeated commands must be sent to reach desired level
                        uint32_t level = x.Data();
                        for (uint32_t i = 0; i < LOOP_CT_LEVEL_ADJ; i++)
                        {
                            self_q.push(FSMEvent(FSMEventCode::E_COM_LEVEL_1, level));
                            self_q.push(FSMEvent(FSMEventCode::E_COM_LEVEL_2, level));
                        }
                        k_time_div = 0;
                        break;
                    }
                    case FSMEventCode::E_COM_XOFF:
                    {
                        // flush any self-queued commands
                        // device's own timer will shut it off
                        self_q.clear();
                        k_time_div = 0;
                        break;
                    }
                    default:
                    {
                        break;
                    }
                }
            }

            if (self_q.empty())
            {
                // periodic ping when idle (self queue is empty)

                k_time_div++;
                if (k_time_div == LOOP_CT_1SEC)
                {
                    k_time_div = 0;
                    if (!send_cmd(hComm, '0', 'a'))
                    {
                        // kill loop if this fails for some reason
                        // like if USB COM adapter gets yanked
                        is_running = false;
                    }
                }
            }
            else
            {
                // handle self-queued events at their own rate

                if (k_time_div == 0)
                {
                    bool result = false;
                    FSMEvent x = self_q.pop();
                    switch (x.Code())
                    {
                        case FSMEventCode::E_COM_XON:
                        {
                            ///@TODO -- FIXME use proper command
                            result = send_cmd(hComm, '0', 'a');
                            break;
                        }
                        case FSMEventCode::E_COM_LEVEL_1:
                        {
                            result = send_cmd(hComm, '1', 'a' + x.Data());
                            break;
                        }
                        case FSMEventCode::E_COM_LEVEL_2:
                        {
                            result = send_cmd(hComm, '2', 'a' + x.Data());
                            break;
                        }
                        default:
                        {
                            break;
                        }
                    }
                }

                k_time_div++;
                if (k_time_div == LOOP_CT_SELF_Q_DRAIN)
                {
                    k_time_div = 0;
                }
            }
        }

        // clean up
    }

    CloseHandle(hComm);

    // report thread termination
    rqtx.push(FSMEvent(FSMEventCode::E_COM_UP, 0));
}
