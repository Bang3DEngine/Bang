#include "Bang/IEventListener.h"

USING_NAMESPACE_BANG

void IEventListenerCommon::SetReceiveEventsCommon(bool receiveEvents)
{
    m_isReceivingEventsCommon = receiveEvents;
}

bool IEventListenerCommon::IsReceivingEventsCommon() const
{
    return m_isReceivingEventsCommon;
}


