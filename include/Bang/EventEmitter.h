#ifndef EVENTEMITTER_H
#define EVENTEMITTER_H

#include <functional>

#include "Bang/Array.h"

namespace Bang
{
template <class>
class EventListener;

template <class T>
class EventEmitter
{
public:
    void SetEmitEvents(bool emitEvents);
    bool IsEmittingEvents() const;

    void RegisterListener(EventListener<T> *listener);
    void UnRegisterListener(EventListener<T> *listener);

    void MarkListenerAsDeleted(EventListener<T> *listener);
    void ClearDeletedListeners();

    template <class TFunction, class... Args>
    void PropagateToArray(const Array<EventListener<T> *> &array,
                          const TFunction &func,
                          const Args &... args) const;

    void PropagateToArrayFunctor(
        const Array<EventListener<T> *> &array,
        std::function<void(EventListener<T> *)> listenerCall) const;

    template <class TFunction, class... Args>
    void PropagateToListeners(const TFunction &func,
                              const Args &... args) const;

    template <class TResult, class TFunction, class... Args>
    Array<TResult> PropagateToListenersAndGatherResult(
        const TFunction &func,
        const Args &... args) const;

    Array<EventListener<T> *> &GetListeners();
    const Array<EventListener<T> *> &GetListeners() const;

protected:
    EventEmitter() = default;
    virtual ~EventEmitter();

private:
    bool m_emitEvents = true;
    mutable int m_iterationDepth = 0;
    Array<EventListener<T> *> m_listeners;
};
}

#include "Bang/EventEmitter.tcc"

#endif  // EVENTEMITTER_H
