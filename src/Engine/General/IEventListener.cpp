#include "Bang/IEventListener.h"

#include "Bang/IEventEmitter.h"

USING_NAMESPACE_BANG

IEventListener::~IEventListener()
{
    m_isBeingDestroyed = true;
    while (!m_emitters.IsEmpty())
    {
        m_emitters.Front()->UnRegisterListener(this);
    }
}

void IEventListener::SetReceiveEvents(bool receiveEvents)
{
    m_receivesEvents = receiveEvents;
}

bool IEventListener::IsReceivingEvents() const
{
    return m_receivesEvents;
}

void IEventListener::OnRegisteredTo(IEventEmitter *emitter)
{
    if (!m_isBeingDestroyed)
    {
        m_emitters.PushBack(emitter);
    }
}

void IEventListener::OnUnRegisteredFrom(IEventEmitter *emitter)
{
    m_emitters.Remove(emitter);
}
