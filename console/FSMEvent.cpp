#include "FSMEvent.h"

FSMEvent::FSMEvent() :
    code(FSMEventCode::E_NONE),
    data(0u),
    str("")
{
}

FSMEvent::FSMEvent(const FSMEventCode c) :
    code(c),
    data(0u),
    str("")
{
}

FSMEvent::FSMEvent(const FSMEventCode c, const uint32_t d) :
    code(c),
    data(d),
    str("")
{
}

FSMEvent::FSMEvent(const FSMEventCode c, const std::string& rs) :
    code(c),
    data(0u),
    str(rs)
{
}

FSMEvent::~FSMEvent()
{
}

const FSMEventCode& FSMEvent::Code() const
{
    return code;
}

const uint32_t& FSMEvent::Data() const
{
    return data;
}

const std::string& FSMEvent::Str() const
{
    return str;
}
