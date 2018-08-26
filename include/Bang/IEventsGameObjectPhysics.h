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
        (void) collision;
    }

    virtual void OnCollisionPersists(const Collision &collision)
    {
        (void) collision;
    }

    virtual void OnCollisionExit(const Collision &collision)
    {
        (void) collision;
    }

    virtual void OnTriggerEnter(Collider *otherCollider)
    {
        (void) otherCollider;
    }

    virtual void OnTriggerExit(Collider *otherCollider)
    {
        (void) otherCollider;
    }
};

NAMESPACE_BANG_END

#endif // IEVENTSGAMEOBJECTPHYSICS_H
