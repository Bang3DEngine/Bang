#ifndef EVENTEMITTER_TCC
#define EVENTEMITTER_TCC

#include "Bang/EventEmitter.h"

USING_NAMESPACE_BANG

template<class T>
EventEmitter<T>::~EventEmitter()
{
    m_mutableIterators.PushBack( MutableIterator(m_listeners.Begin()) );
    MutableIterator &mIt = m_mutableIterators.Back();
    while (mIt.GetIterator() != GetListeners().End())
    {
        EventListener<T> *listener = *(mIt.GetIterator());
        UnRegisterListener(listener);

        // Recheck needed, in case it was modified
        if (mIt.GetIterator() != GetListeners().End())
        {
            mIt.IncreaseIfNeeded();
        }
    }
    m_mutableIterators.PopBack();
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
void EventEmitter<T>::RegisterListener(EventListener<T> *listener)
{
    if (!m_listeners.Contains(listener))
    {
        m_listeners.PushBack(listener);
        listener->AddEmitter(this);
    }
}

template <class T>
void EventEmitter<T>::UnRegisterListener(EventListener<T> *listener)
{
    auto listenerIt = m_listeners.Find(listener);
    if (listenerIt != m_listeners.End())
    {
        auto nextListenerIt = m_listeners.Remove(listenerIt);
        listener->RemoveEmitter(this);
        for (auto &mIt : m_mutableIterators)
        {
            if (mIt.GetIterator() == listenerIt)
            {
                mIt.SetIterator(nextListenerIt);
            }
        }
    }
}

template<class T>
template<class TFunction, class... Args>
void EventEmitter<T>::PropagateToListeners_(
              std::function<void(EventListener<T>*)> listenerCall,
              const TFunction &func,
              const Args&... args) const
{
    EventEmitter<T> *ncThis = const_cast< EventEmitter<T>* >(this);
    m_mutableIterators.PushBack( MutableIterator(ncThis->m_listeners.Begin()) );

    MutableIterator &mIt = m_mutableIterators.Back();
    while (mIt.GetIterator() != GetListeners().End())
    {
        if (IsEmittingEvents())
        {
            EventListener<T>* listener = *(mIt.GetIterator());
            if (listener && listener->IsReceivingEvents())
            {
                listenerCall(listener);
            }
        }

        // Recheck needed, in case it was modified
        if (mIt.GetIterator() != GetListeners().End())
        {
            mIt.IncreaseIfNeeded();
        }
    }
    m_mutableIterators.PopBack();
}

template <class T>
template<class TFunction, class... Args>
void EventEmitter<T>::
PropagateToListeners(const TFunction &func, const Args&... args) const
{
    PropagateToListeners_([&](EventListener<T> *listener)
                          {
                              (listener->*func)(args...);
                          },
                          func, args...);
}

template<class T>
template<class TResult, class TFunction, class... Args>
Array<TResult>
EventEmitter<T>::PropagateToListenersAndGatherResult(const TFunction &func,
                                                     const Args&... args) const
{
    Array<TResult> gatheredResult;
    PropagateToListeners_([&](EventListener<T> *listener)
                          {
                              gatheredResult.PushBack( (listener->*func)(args...) );
                          },
                          func, args...);
    return gatheredResult;
}

template<class T>
const List<EventListener<T>*>& EventEmitter<T>::GetListeners() const
{
    return m_listeners;
}

#endif // EVENTEMITTER_TCC
