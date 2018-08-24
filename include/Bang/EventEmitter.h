#ifndef EVENTEMITTER_H
#define EVENTEMITTER_H

#include <functional>

#include "Bang/List.h"
#include "Bang/USet.h"
#include "Bang/EventEmitter.h"

NAMESPACE_BANG_BEGIN

FORWARD template <class T> class EventListener;

template <class T>
class EventEmitter
{
public:
    void SetEmitEvents(bool emitEvents);
    bool IsEmittingEvents() const;

    bool RegisterListener(EventListener<T> *listener);
    bool UnRegisterListener(EventListener<T> *listener);

    template<class TFunction, class... Args>
    void PropagateToListeners(const TFunction &func, const Args&... args) const;

    template<class TResult, class TFunction, class... Args>
    Array<TResult> PropagateToListenersAndGatherResult(const TFunction &func,
                                                       const Args&... args) const;

    bool IsIteratingListeners() const;
    const List<EventListener<T>*>& GetListeners() const;

protected:
    EventEmitter() = default;
    virtual ~EventEmitter();

private:
    bool m_emitEvents = true;
    List<EventListener<T>*> m_listeners;

    struct MutableIterator
    {
    public:
        using TIterator = typename List<EventListener<T>*>::Iterator;
        MutableIterator(TIterator it) : m_it(it)
        {
        }

        TIterator GetIterator()
        {
            return m_it;
        }

        void SetIterator(TIterator it)
        {
            m_hasBeenModified = true;
            m_it = it;
        }

        void IncreaseIfNeeded()
        {
            if (!m_hasBeenModified)
            {
                ++m_it;
            }
            else
            {
                m_hasBeenModified = false;
            }
        }
    private:
        bool m_hasBeenModified = false;
        TIterator m_it;
    };
    mutable List<MutableIterator> m_mutableIterators;

    template<class TFunction, class... Args>
    void PropagateToListeners_(
                std::function<void(EventListener<T>*)> listenerCall,
                const TFunction &func,
                const Args&... args) const;
};

NAMESPACE_BANG_END

#include "Bang/EventEmitter.tcc"

#endif // EVENTEMITTER_H
