#include "Bang/IEventListenerCommon.h"

using namespace Bang;

void IEventListenerCommon::SetReceiveEventsCommon(bool receiveEvents)
{
    m_isReceivingEventsCommon = receiveEvents;
}

bool IEventListenerCommon::IsReceivingEventsCommon() const
{
    return m_isReceivingEventsCommon;
}
