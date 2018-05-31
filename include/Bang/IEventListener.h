#ifndef IEVENTLISTENER_H
#define IEVENTLISTENER_H

#include "Bang/Bang.h"

NAMESPACE_BANG_BEGIN

FORWARD class IEventEmitter;

#define EVENTLISTENER_NS(ClassName) \
    private: \
        ClassName() = default; \
        virtual ~ClassName() = default; \
    template<class> friend class Bang::EventListener;

#define EVENTLISTENER(ClassName) \
    private: \
        ClassName() = default; \
        virtual ~ClassName() = default; \
    template<class> friend class EventListener;

class IEventListenerCommon
{
protected:
    IEventListenerCommon() = default;
    virtual ~IEventListenerCommon() = default;

public:
    void SetReceiveEventsCommon(bool receiveEvents);
    bool IsReceivingEventsCommon() const;

private:
    bool m_isReceivingEventsCommon = true;
};

class IEventListener
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

