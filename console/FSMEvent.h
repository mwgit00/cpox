#ifndef FSM_EVENT_H_
#define FSM_EVENT_H_

#include <cstdint>
#include <string>
#include <list>

#include "MTQueue.hpp"


enum class FSMEventCode : uint32_t
{
    E_NONE =        0x00u,  ///< blank event
    E_KEY =         0x01u,  ///< key pressed
    E_CVOK =        0x02u,  ///< valid face / eye detection
    E_TMR_CV =      0x03u,  ///< computer vision state machine timer expired
    E_TMR_SR =      0x04u,  ///< speech rec. state machine timer expired
    E_SR_FAIL =     0x10u,  ///< speech rec. failure (based on strikes)
    E_SR_REC =      0x11u,  ///< begin recognition
    E_SR_STOP =     0x12u,  ///< stop phrase machine
    E_SR_RESTART =  0x13u,  ///< restart phrase machine
    E_SR_STRIKES =  0x14u,  ///< speech rec. strike count back to app
    E_SR_PHRASE =   0x15u,  ///< say phrase to be repeated and recognized
    E_SR_RESULT =   0x16u,  ///< speech rec. returned pass-fail result
    E_SR_RESET =    0x17u,  ///< speech rec. back to idle
    E_TTS_UP =      0x20u,  ///< TTS task initialized successfully
    E_TTS_IDLE =    0x22u,  ///< TTS task has finished speaking a phrase
    E_COM_UP =      0x40u,  ///< COM task initialized successfully
    E_COM_XON =     0x41u,  ///< COM external action begin
    E_COM_XOFF =    0x42u,  ///< COM external action end
    E_COM_ACK =     0x43u,  ///< COM task sent ack/response
    E_COM_LEVEL =   0x44u,  ///< COM external action level adjustment
    E_COM_LEVEL_1 = 0x45u,  ///< COM external action (output 1) level adjustment
    E_COM_LEVEL_2 = 0x46u,  ///< COM external action (output 2) level adjustment
    E_UDP_UP =      0x80u,  ///< UDP task initialized successfully
    E_UDP_SAY =     0x81u,  ///< UDP task command to say phrase
    E_UDP_LOAD =    0x82u,  ///< UDP task command to load phrase
    E_UDP_REPEAT =  0x83u,  ///< UDP task command to repeat loaded phrase
    E_UDP_REC =     0x84u,  ///< UDP task command to recognize loaded phrase
    E_UDP_TTS_OK =  0x85u,  ///< UDP task finished speaking phrase
    E_UDP_REC_VAL = 0x86u,  ///< UDP task recognition result value
    E_TASK_HALT =   0xF0u,  ///< tell worker task to terminate
};

class FSMEvent
{
public:
    
    FSMEvent();
    explicit FSMEvent(const FSMEventCode c);
    explicit FSMEvent(const FSMEventCode c, const uint32_t d);
    explicit FSMEvent(const FSMEventCode c, const std::string& rs);
    virtual ~FSMEvent();

    const FSMEventCode& Code() const;
    const uint32_t& Data() const;
    const std::string& Str() const;

private:
    FSMEventCode code;
    uint32_t data;
    std::string str;
};

typedef std::list<FSMEvent> tListEvent;
typedef MTQueue<FSMEvent> tEventQueue;

#endif // FSM_EVENT_H_
