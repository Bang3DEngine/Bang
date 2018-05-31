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
        EventListener<T> *eListener = DCAST<EventListener<T>*>( m_listeners.Front() );
        UnRegisterListener(eListener);
    }
}

template <class T>
void EventEmitter<T>::RegisterListener(EventListener<T> *listener)
{
    if (!m_isBeingDestroyed)
    {
        if (!IsIteratingListeners())
        {
            if (!m_listeners.Contains(listener))
            {
                m_listeners.PushBack(listener);
                if (listener) { listener->OnRegisteredTo(this); }
            }
        }
        else { m_delayedListenersToRegister.PushBack(listener); }
    }
}

template <class T>
void EventEmitter<T>::UnRegisterListener(EventListener<T> *listener)
{
    if (!IsIteratingListeners())
    {
        m_listeners.Remove(listener);
        if (listener) { listener->OnUnRegisteredFrom(this); }
    }
    else { m_delayedListenersToUnRegister.PushBack(listener); }
}

template<class T>
bool EventEmitter<T>::IsIteratingListeners() const
{
    ASSERT(m_iteratingListeners >= 0);
    return (m_iteratingListeners > 0);
}

template <class T>
template<class TListener, class TFunction, class... Args>
void EventEmitter<T>::
PropagateToListener(const TListener &listener, const TFunction &func,
                    const Args&... args)
{
    if (listener && listener->IsReceivingEvents())
    {
        (DCAST<EventListener<T>*>(listener)->*func)(args...);
    }
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
            ncThis->RegisterListener( DCAST<EventListener<T>*>(listener) );
        }
        for (EventListener<T> *listener : m_delayedListenersToUnRegister)
        {
            ncThis->UnRegisterListener( DCAST<EventListener<T>*>(listener) );
        }
        m_delayedListenersToRegister.Clear();
        m_delayedListenersToUnRegister.Clear();
    }

    ++m_iteratingListeners;
    for (const auto &x : GetListeners())
    {
        #ifdef DEBUG
        const int previousSize = GetListeners().Size(); (void) previousSize;
        #endif

        if (IsEmittingEvents()) { PropagateToListener(x, func, args...); }

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
