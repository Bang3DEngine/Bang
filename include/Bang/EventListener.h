#ifndef EVENTLISTENER_H
#define EVENTLISTENER_H

#include "Bang/IEventListenerCommon.h"

NAMESPACE_BANG_BEGIN

FORWARD template<class T> class EventEmitter;

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
    bool m_isBeingDestroyed = false;
    List<EventEmitter<T>*> m_emitters;

    void ClearRegistrations();
    void OnRegisteredTo(EventEmitter<T> *emitter);
    void OnUnRegisteredFrom(EventEmitter<T> *emitter);

    template<class> friend class EventEmitter;
};

NAMESPACE_BANG_END

#include "Bang/EventListener.tcc"

#endif // EVENTLISTENER_H

