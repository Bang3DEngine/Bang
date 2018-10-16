#ifndef EVENTLISTENER_H
#define EVENTLISTENER_H

#include "Bang/IEventListenerCommon.h"

NAMESPACE_BANG_BEGIN

FORWARD_T class EventEmitter;

template <class T>
class EventListener : public T,
                      public virtual IEventListenerCommon
{
public:
    void SetReceiveEvents(bool receiveEvents);
    bool IsReceivingEventsNonCommon() const;
    bool IsReceivingEvents() const;

protected:
    EventListener() = default;
    virtual ~EventListener();

private:
    bool m_receivesEvents = true;
    Array<EventEmitter<T>*> m_emitters;

    void AddEmitter(EventEmitter<T> *emitter);
    void RemoveEmitter(EventEmitter<T> *emitter);

    template<class> friend class EventEmitter;
};

NAMESPACE_BANG_END

#include "Bang/EventListener.tcc"

#endif // EVENTLISTENER_H

