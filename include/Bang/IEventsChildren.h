#ifndef IEVENTSCHILDREN_H
#define IEVENTSCHILDREN_H

#include "Bang/IEvents.h"

NAMESPACE_BANG_BEGIN

FORWARD class GameObject;

class IEventsChildren
{
    IEVENTS(IEventsChildren);

public:
    virtual void OnChildAdded(GameObject *addedChild, GameObject *parent)
    {
        (void) addedChild;
        (void) parent;
    }

    virtual void OnChildRemoved(GameObject *removedChild, GameObject *parent)
    {
        (void) removedChild;
        (void) parent;
    }

    virtual void OnParentChanged(GameObject *oldParent, GameObject *newParent)
    {
        (void) oldParent;
        (void) newParent;
    }
};

NAMESPACE_BANG_END

#endif // IEVENTSCHILDREN_H
