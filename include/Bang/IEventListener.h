#ifndef IEVENTLISTENER_H
#define IEVENTLISTENER_H

#include "Bang/Bang.h"
#include "Bang/IEventListenerCommon.h"

NAMESPACE_BANG_BEGIN

FORWARD class IEventEmitter;

class IEventListener : public virtual IEventListenerCommon
{
public:
    virtual void SetReceiveEvents(bool receiveEvents) = 0;
    virtual bool IsReceivingEvents() const = 0;

protected:
    IEventListener() = default;
    virtual ~IEventListener() = default;

    virtual void OnRegisteredTo(IEventEmitter *emitter) = 0;
    virtual void OnUnRegisteredFrom(IEventEmitter *emitter) = 0;

    template<class> friend class EventListener;
};

NAMESPACE_BANG_END

#endif // IEVENTLISTENER_H

