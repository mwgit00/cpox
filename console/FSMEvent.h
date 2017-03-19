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
    E_TTS_SAY =     0x20u,  ///< TTS task commanded to say a phrase
    E_TTS_IDLE =    0x21u,  ///< TTS task has finished speaking a phrase
    E_TTS_UP =      0x22u,  ///< TTS task initialized successfully
    E_COM_XON =     0x40u,  ///< COM external action begin
    E_COM_XOFF =    0x41u,  ///< COM external action end
    E_COM_UP =      0x42u,  ///< COM task initialized successfully
    E_COM_ACK =     0x43u,  ///< COM task sent ack/response
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
