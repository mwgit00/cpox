#ifndef FSM_EVENT_H_
#define FSM_EVENT_H_

#include <cstdint>
#include <string>
#include <list>

#include "MTQueue.hpp"


enum class FSMEventCode : uint32_t
{
    E_NONE =    0x00u,  ///< blank event
    E_KEY =     0x01u,  ///< key pressed
    E_TMR_CV =  0x02u,  ///< computer vision state machine timer expired
    E_CVOK =    0x03u,  ///< valid face / eye detection
    E_SRFAIL =  0x04u,  ///< speech recognition failure(based on strikes)
    E_TMR_SR =  0x05u,  ///< speech rec.state machine timer expired
    E_SRGO =    0x06u,  ///< speech recognition begin
    E_STOP =    0x07u,  ///< stop phrase machine
    E_GO =      0x08u,  ///< restart phrase machine
    E_SRACK =   0x09u,  ///< speech recognition result has been processed
    E_XON =     0x10u,  ///< external action begin
    E_XOFF =    0x11u,  ///< external action end
    E_SAY =     0x12u,  ///< say a phrase
    E_SAY_REP = 0x13u,  ///< say a canned phrase to be repeated
    E_SDONE =   0x20u,  ///< spoken phrase done
    E_RDONE =   0x21u,  ///< recognition done

    E_CMD_HALT =    0x100u,
};

class FSMEvent
{
public:
    FSMEvent();
    FSMEvent(const FSMEventCode c);
    FSMEvent(const FSMEventCode c, const uint32_t d);
    FSMEvent(const FSMEventCode c, const std::string& rs);
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
