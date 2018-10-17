#ifndef IEVENTSGAMEOBJECTPHYSICS_H
#define IEVENTSGAMEOBJECTPHYSICS_H

#include "Bang/Collision.h"
#include "Bang/IEvents.h"

namespace Bang
{
class Collider;

class IEventsGameObjectPhysics
{
    IEVENTS(IEventsGameObjectPhysics);

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
}

#endif  // IEVENTSGAMEOBJECTPHYSICS_H
