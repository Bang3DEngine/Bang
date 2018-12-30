#ifndef OBJECT_H
#define OBJECT_H

#include <vector>

#include "Bang/Array.tcc"
#include "Bang/BangDefines.h"
#include "Bang/EventEmitter.h"
#include "Bang/EventEmitter.tcc"
#include "Bang/EventListener.h"
#include "Bang/EventListener.tcc"
#include "Bang/ICloneable.h"
#include "Bang/IEventsDestroy.h"
#include "Bang/IEventsObject.h"
#include "Bang/ObjectId.h"
#include "Bang/ObjectMacros.h"
#include "Bang/Serializable.h"

namespace Bang
{
class Object : public Serializable,
               public EventListener<IEventsObject>,
               public EventEmitterIEventsDestroyWithCheck,
               public EventEmitter<IEventsObject>
{
    SET_CLASS_ID_AS_ROOT(Object)

public:
    const ObjectId &GetObjectId() const;
    void SetEnabled(bool enabled);

    bool IsActive() const;
    bool IsEnabled() const;
    bool IsStarted() const;
    bool IsActiveRecursively() const;
    bool IsEnabledRecursively() const;
    bool IsBeingDestroyed() const;
    bool IsWaitingToBeDestroyed() const;

    // ICloneable
    virtual void CloneInto(ICloneable *clone, bool cloneGUID) const override;

protected:
    Object();
    virtual ~Object() override;

    virtual void PreStart();
    virtual void Start();

    virtual void OnPreStart();
    virtual void OnStart();
    virtual void OnEnabled(Object *object) override;
    virtual void OnDisabled(Object *object) override;
    virtual void OnDestroy();

    void SetBeingDestroyed();
    void SetWaitingToBeDestroyed();
    void InvalidateEnabledRecursively();

    static void PropagateObjectDestruction(Object *object);

protected:
    bool GetDestroyEventHasBeenPropagated() const;

private:
    ObjectId m_objectId;
    bool m_enabled = true;
    bool m_started = false;

    bool m_beingDestroyed = false;
    bool m_waitingToBeDestroyed = false;
    bool m_destroyEventPropagated = false;

    mutable bool m_enabledRecursivelyValid = false;
    mutable bool m_enabledRecursively = false;

    virtual bool CalculateEnabledRecursively() const = 0;
    virtual void OnEnabledRecursivelyInvalidated();
};
}  // namespace Bang

#endif  // OBJECT_H
