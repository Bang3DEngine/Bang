#ifndef EVENTEMITTER_H
#define EVENTEMITTER_H

#include "Bang/List.h"
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

    bool IsIteratingListeners() const;
    const List<EventListener<T>*>& GetListeners() const;

protected:
    EventEmitter() = default;
    virtual ~EventEmitter();

private:
    bool m_emitEvents = true;
    bool m_isBeingDestroyed = false;
    List<EventListener<T>*> m_listeners;

    mutable int m_iteratingListeners = 0;
    mutable List<EventListener<T>*> m_delayedListenersToRegister;
    mutable List<EventListener<T>*> m_delayedListenersToUnRegister;
};

NAMESPACE_BANG_END

#include "Bang/EventEmitter.tcc"

#endif // EVENTEMITTER_H
