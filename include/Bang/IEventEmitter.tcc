#ifndef IEVENTEMITTER_TCC
#define IEVENTEMITTER_TCC

#include "Bang/IEventEmitter.h"

USING_NAMESPACE_BANG

template<class EventListenerClass>
EventEmitter<EventListenerClass>::~EventEmitter()
{
    m_isBeingDestroyed = true;
    while (!m_listeners.IsEmpty())
    {
        IEventListener *eListener = DCAST<IEventListener*>( m_listeners.Front() );
        UnRegisterListener(eListener);
    }
}

template <class EListenerC>
void EventEmitter<EListenerC>::RegisterListener(EListenerC *listener)
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

template <class EListenerC>
void EventEmitter<EListenerC>::UnRegisterListener(IEventListener *listener)
{
    if (!IsIteratingListeners())
    {
        m_listeners.Remove(listener);
        if (listener) { listener->OnUnRegisteredFrom(this); }
    }
    else { m_delayedListenersToUnRegister.PushBack(listener); }
}

template<class EventListenerClass>
bool EventEmitter<EventListenerClass>::IsIteratingListeners() const
{
    ASSERT(m_iteratingListeners >= 0);
    return (m_iteratingListeners > 0);
}

template <class EventListenerClass>
template<class TListener, class TFunction, class... Args>
void EventEmitter<EventListenerClass>::
PropagateToListener(const TListener &listener, const TFunction &func,
                    const Args&... args)
{
    if (listener && listener->IsReceivingEvents())
    {
        (DCAST<EventListenerClass*>(listener)->*func)(args...);
    }
}

template <class EventListenerClass>
template<class TFunction, class... Args>
void EventEmitter<EventListenerClass>::
PropagateToListeners(const TFunction &func, const Args&... args) const
{
    // Un/Register delayed listeners, if not iterating
    if ( !IsIteratingListeners() )
    {
        EventEmitter<EventListenerClass> *ncThis =
                        const_cast< EventEmitter<EventListenerClass>* >(this);
        for (IEventListener *listener : m_delayedListenersToRegister)
        {
            ncThis->RegisterListener( DCAST<EventListenerClass*>(listener) );
        }
        for (IEventListener *listener : m_delayedListenersToUnRegister)
        {
            ncThis->UnRegisterListener( DCAST<EventListenerClass*>(listener) );
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


template<class EventListenerClass>
const List<IEventListener*>& EventEmitter<EventListenerClass>::GetListeners() const
{
    return m_listeners;
}

#endif // IEVENTEMITTER_TCC
