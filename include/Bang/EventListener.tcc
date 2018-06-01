#ifndef EVENTLISTENER_TCC
#define EVENTLISTENER_TCC

#include "Bang/EventListener.h"

USING_NAMESPACE_BANG

template<class T>
EventListener<T>::~EventListener()
{
    m_isBeingDestroyed = true;
    while (!m_emitters.IsEmpty())
    {
        m_emitters.Front()->UnRegisterListener(this);
    }
}

template<class T>
void EventListener<T>::SetReceiveEvents(bool receiveEvents)
{
    m_receivesEvents = receiveEvents;
}

template<class T>
bool EventListener<T>::IsReceivingEventsNonCommon() const
{
    return m_receivesEvents;
}

template<class T>
bool EventListener<T>::IsReceivingEvents() const
{
    return IsReceivingEventsNonCommon() && IsReceivingEventsCommon();
}

template<class T>
void EventListener<T>::OnRegisteredTo(EventEmitter<T> *emitter)
{
    if (!m_isBeingDestroyed)
    {
        m_emitters.PushBack(emitter);
    }
}

template<class T>
void EventListener<T>::OnUnRegisteredFrom(EventEmitter<T> *emitter)
{
    m_emitters.Remove(emitter);
}

#endif // EVENTLISTENER_TCC
