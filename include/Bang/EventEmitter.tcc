#ifndef EVENTEMITTER_TCC
#define EVENTEMITTER_TCC

#include "Bang/EventEmitter.h"

USING_NAMESPACE_BANG

template<class T>
EventEmitter<T>::~EventEmitter()
{
    m_isBeingDestroyed = true;
    while (!m_listeners.IsEmpty())
    {
        UnRegisterListener( m_listeners.Front() );
    }
}

template<class T>
void EventEmitter<T>::SetEmitEvents(bool emitEvents)
{
    m_emitEvents = emitEvents;
}

template<class T>
bool EventEmitter<T>::IsEmittingEvents() const
{
    return m_emitEvents;
}

template <class T>
bool EventEmitter<T>::RegisterListener(EventListener<T> *listener)
{
    if (!m_isBeingDestroyed)
    {
        if (!IsIteratingListeners())
        {
            if (!m_listeners.Contains(listener))
            {
                m_listeners.PushBack(listener);
                if (listener)
                {
                    listener->OnRegisteredTo(this);
                }
                return true;
            }
        }
        else
        {
            m_delayedListenersToRegister.PushBack(listener);
        }
    }
    return false;
}

template <class T>
bool EventEmitter<T>::UnRegisterListener(EventListener<T> *listener)
{
    if (!IsIteratingListeners())
    {
        m_listeners.Remove(listener);
        if (listener)
        {
            listener->OnUnRegisteredFrom(this);
        }
        return true;
    }
    else
    {
        m_delayedListenersToUnRegister.PushBack(listener);
        return false;
    }
}

template<class T>
bool EventEmitter<T>::IsIteratingListeners() const
{
    ASSERT(m_iteratingListeners >= 0);
    return (m_iteratingListeners > 0);
}

template <class T>
template<class TFunction, class... Args>
void EventEmitter<T>::
PropagateToListeners(const TFunction &func, const Args&... args) const
{
    // Un/Register delayed listeners, if not iterating
    if ( !IsIteratingListeners() )
    {
        EventEmitter<T> *ncThis = const_cast< EventEmitter<T>* >(this);
        for (EventListener<T> *listener : m_delayedListenersToRegister)
        {
            ncThis->RegisterListener(listener);
        }
        for (EventListener<T> *listener : m_delayedListenersToUnRegister)
        {
            ncThis->UnRegisterListener(listener);
        }
        m_delayedListenersToRegister.Clear();
        m_delayedListenersToUnRegister.Clear();
    }

    ++m_iteratingListeners;
    for (const auto &listener : GetListeners())
    {
        #ifdef DEBUG
        const int previousSize = GetListeners().Size(); (void) previousSize;
        #endif

        if (IsEmittingEvents())
        {
            if (listener && listener->IsReceivingEvents())
            {
                (listener->*func)(args...);
            }
        }

        ASSERT(GetListeners().Size() == previousSize);
    }
    --m_iteratingListeners;
}


template<class T>
const List<EventListener<T>*>& EventEmitter<T>::GetListeners() const
{
    return m_listeners;
}

#endif // EVENTEMITTER_TCC
