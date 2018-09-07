#ifndef IEVENTSCHILDREN_H
#define IEVENTSCHILDREN_H

#include "Bang/IEvents.h"

NAMESPACE_BANG_BEGIN

FORWARD class GameObject;

class IEventsChildren
{
    IEVENTS(IEventsChildren)

public:
    virtual void OnChildAdded(GameObject *addedChild, GameObject *parent)
    {
        BANG_UNUSED_2(addedChild, parent);
    }

    virtual void OnChildRemoved(GameObject *removedChild, GameObject *parent)
    {
        BANG_UNUSED_2(removedChild, parent);
    }

    virtual void OnParentChanged(GameObject *oldParent, GameObject *newParent)
    {
        BANG_UNUSED_2(oldParent, newParent);
    }
};

NAMESPACE_BANG_END

#endif // IEVENTSCHILDREN_H
