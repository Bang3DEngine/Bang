#ifndef EVENTLISTENER_H
#define EVENTLISTENER_H

#include "Bang/IEventListener.h"

NAMESPACE_BANG_BEGIN

FORWARD template<class T> class EventEmitter;

template <class T>
class EventListener : public T,
                      public IEventListener
{
public:
    void SetReceiveEvents(bool receiveEvents);
    bool IsReceivingEvents() const;

protected:
    EventListener();
    ~EventListener();

private:
    bool m_receivesEvents = true;
    bool m_isBeingDestroyed = false;
    List<EventEmitter<T>*> m_emitters;

    void OnRegisteredTo(IEventEmitter *emitter) override;
    void OnUnRegisteredFrom(IEventEmitter *emitter) override;

    template<class> friend class EventEmitter;
};

NAMESPACE_BANG_END

#include "Bang/EventListener.tcc"

#endif // EVENTLISTENER_H

