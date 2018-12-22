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
        m_destroyChecker.p_objectBeingChecked = this;
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
        EventEmitterIEventsDestroyWithCheck *p_objectBeingChecked = nullptr;
        void OnDestroyed(EventEmitter<IEventsDestroy> *object) override
        {
            ASSERT(p_objectBeingChecked);
            ASSERT(object == p_objectBeingChecked);
            p_objectBeingChecked->m_destroyedCalled = true;
        }
    };

    bool m_destroyedCalled = false;
    DestroyChecker m_destroyChecker;
};
}

#endif  // IEVENTSDESTROY_H
