#pragma once

#include "Bang/EventListener.h"

using namespace Bang;

template <class T>
EventListener<T>::~EventListener()
{
    while (!m_emitters.IsEmpty())
    {
        m_emitters.Front()->UnRegisterListener(this);
    }
}

template <class T>
void EventListener<T>::SetReceiveEvents(bool receiveEvents)
{
    m_receivesEvents = receiveEvents;
}

template <class T>
bool EventListener<T>::IsReceivingEventsNonCommon() const
{
    return m_receivesEvents;
}

template <class T>
bool EventListener<T>::IsReceivingEvents() const
{
    return IsReceivingEventsNonCommon() && IsReceivingEventsCommon();
}

template <class T>
void EventListener<T>::AddEmitter(EventEmitter<T> *emitter)
{
    if (!m_emitters.Contains(emitter))
    {
        m_emitters.PushBack(emitter);
    }
}

template <class T>
void EventListener<T>::RemoveEmitter(EventEmitter<T> *emitter)
{
    m_emitters.Remove(emitter);
}
