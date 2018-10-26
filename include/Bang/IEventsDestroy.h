#ifndef IEVENTSDESTROY_H
#define IEVENTSDESTROY_H

#include "Bang/Assert.h"
#include "Bang/EventEmitter.h"
#include "Bang/EventListener.h"
#include "Bang/IEvents.h"

namespace Bang
{
class IEventsDestroy
{
    IEVENTS(IEventsDestroy);

public:
    virtual void OnDestroyed(EventEmitter<IEventsDestroy> *object)
    {
        BANG_UNUSED(object);
    }
};

class EventEmitterIEventsDestroyWithCheck : public EventEmitter<IEventsDestroy>
{
public:
    EventEmitterIEventsDestroyWithCheck()
    {
        m_destroyChecker.p_destroyEmitter = this;
        RegisterListener(&m_destroyChecker);
    }

    virtual ~EventEmitterIEventsDestroyWithCheck()
    {
        ASSERT(m_destroyedCalled);
    }

private:
    class DestroyChecker : public EventListener<IEventsDestroy>
    {
    public:
        EventEmitterIEventsDestroyWithCheck *p_destroyEmitter = nullptr;
        void OnDestroyed(EventEmitter<IEventsDestroy> *object)
        {
            ASSERT(p_destroyEmitter);
            ASSERT(object == p_destroyEmitter);
            p_destroyEmitter->m_destroyedCalled = true;
        }
    };

    bool m_destroyedCalled = false;
    DestroyChecker m_destroyChecker;
};
}

#endif  // IEVENTSDESTROY_H
