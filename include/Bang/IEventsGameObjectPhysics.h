#ifndef IEVENTSGAMEOBJECTPHYSICS_H
#define IEVENTSGAMEOBJECTPHYSICS_H

#include "Bang/IEvents.h"
#include "Bang/Collision.h"

NAMESPACE_BANG_BEGIN

FORWARD class Collider;

class IEventsGameObjectPhysics
{
    IEVENTS(IEventsGameObjectPhysics)

public:
    virtual void OnCollisionEnter(const Collision &collision)
    {
        BANG_UNUSED(collision);
    }

    virtual void OnCollisionPersists(const Collision &collision)
    {
        BANG_UNUSED(collision);
    }

    virtual void OnCollisionExit(const Collision &collision)
    {
        BANG_UNUSED(collision);
    }

    virtual void OnTriggerEnter(Collider *otherCollider)
    {
        BANG_UNUSED(otherCollider);
    }

    virtual void OnTriggerExit(Collider *otherCollider)
    {
        BANG_UNUSED(otherCollider);
    }
};

NAMESPACE_BANG_END

#endif // IEVENTSGAMEOBJECTPHYSICS_H
