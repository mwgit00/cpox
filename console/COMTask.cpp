#include "Windows.h"

#include <string>
#include "COMTask.h"


// delay per iteration of sleepy com loop (100Hz)
#define LOOP_DELAY_MS           (10)

// number of loop counts per second, determines "heartbeat" rate
#define LOOP_CT_1SEC            (1000 / LOOP_DELAY_MS)

// number of loop counts before self-queued commands are popped
// commands on queue should not be drained faster than 10Hz rate
// this is a limitation of the external device
#define LOOP_CT_SELF_Q_DRAIN    (100 / LOOP_DELAY_MS)

// number of commands that are queued to adjust output levels
#define LOOP_CT_LEVEL_ADJ       (15)

// number of commands to achieve maximum on time of 10 seconds
#define LOOP_MAX_ON_CT_SEC      (10 * LOOP_CT_SELF_Q_DRAIN)


enum
{
    STATE_IDLE = 0,
    STATE_RESP,
    STATE_CMD,
};


/// state variable for parsing incoming commands
static int com_state = STATE_IDLE;

/// buffer for sending an ACK response
static std::string s_ack = "???";

/// queue for storing a sequence of many commands to external device
static tEventQueue self_q;


// command is of form (C[0-3][a-y])
// returns true if TX through port is OK

bool send_cmd(HANDLE hComm, const char c_cmd, const char c_param)
{
    char buff[8];
    DWORD nwritten;
    buff[0] = CMD_SYNC;
    buff[1] = c_cmd;
    buff[2] = c_param;
    BOOL tx_result = WriteFile(hComm, buff, 3, &nwritten, NULL);
    return (tx_result) ? true : false;
}


// response is of form (R[0-3][a-y])
// returns a 1 in smval if valid response found

void com_crank(char c, int& smval)
{
    if (com_state == STATE_IDLE)
    {
        if (c == CMD_RESP)
        {
            s_ack[0] = c;
            com_state = STATE_RESP;
        }
    }
    else if (com_state == STATE_RESP)
    {
        if ((c >= CMD_PING) || (c <= CMD_OUTPUT_ON))
        {
            s_ack[1] = c;
            com_state = STATE_CMD;
        }
        else if (c == CMD_RESP)
        {
            // unexpected start of response
            // but could be out of sync so treat it as new response
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
        if ((c >= CMD_VAL_MIN) && (c <= CMD_VAL_MAX))
        {
            // valid value found
            s_ack[2] = c;
            smval = 1;
        }

        // always go back to idle state
        com_state = STATE_IDLE;
    }
}


void com_task_func(
    const std::string& rsPortName,
    tEventQueue& rqrx,
    tEventQueue& rqtx)
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

    // do unicode name shenanigans
    // to get name for port that can be used below
    std::wstring ws(rsPortName.begin(), rsPortName.end());

    hComm = CreateFile(ws.c_str(),
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
            // this call will return immediately
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
            
            // now that receiving is done
            // check if any commands were received from application
            // sequences of commands for the external device may be queued

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
                        ct = (ct > LOOP_MAX_ON_CT_SEC) ? LOOP_MAX_ON_CT_SEC : ct;
                        for (uint32_t i = 0; i < ct; i++)
                        {
                            self_q.push(FSMEvent(FSMEventCode::E_COM_XON));
                        }
                        k_time_div = 0; 
                        break;
                    }
                    case FSMEventCode::E_COM_LEVEL:
                    {
                        // queue up fixed number of LEVEL commands for external device
                        // each output will need its own commands queued for it
                        // repeated commands must be sent to reach desired level
                        uint32_t external_output_level = x.Data();
                        for (uint32_t i = 0; i < LOOP_CT_LEVEL_ADJ; i++)
                        {
                            self_q.push(FSMEvent(FSMEventCode::E_COM_LEVEL_1, external_output_level));
                            self_q.push(FSMEvent(FSMEventCode::E_COM_LEVEL_2, external_output_level));
                        }
                        k_time_div = 0;
                        break;
                    }
                    case FSMEventCode::E_COM_XOFF:
                    {
                        // flush any self-queued commands
                        // external device's own timer will shut it off
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

            // now check timer for periodic ping
            // and check self queue to send commands to external device

            if (self_q.empty())
            {
                // periodic ping when idle (self queue is empty)

                k_time_div++;
                if (k_time_div == LOOP_CT_1SEC)
                {
                    k_time_div = 0;
                    if (!send_cmd(hComm, CMD_PING, CMD_VAL_MIN))
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
                    char c_level = static_cast<char>(x.Data()) + CMD_VAL_MIN;
                    switch (x.Code())
                    {
                        case FSMEventCode::E_COM_XON:
                        {
                            result = send_cmd(hComm, CMD_OUTPUT_ON, CMD_VAL_MIN);
                            break;
                        }
                        case FSMEventCode::E_COM_LEVEL_1:
                        {
                            result = send_cmd(hComm, CMD_LEVEL_1, c_level);
                            break;
                        }
                        case FSMEventCode::E_COM_LEVEL_2:
                        {
                            result = send_cmd(hComm, CMD_LEVEL_2, c_level);
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
