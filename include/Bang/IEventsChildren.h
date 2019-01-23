#ifndef IEVENTSCHILDREN_H
#define IEVENTSCHILDREN_H

#include "Bang/IEvents.h"

namespace Bang
{
class GameObject;

class IEventsChildren
{
    IEVENTS(IEventsChildren);

public:
    virtual void OnChildAdded(GameObject *addedChild, GameObject *parent)
    {
        BANG_UNUSED_2(addedChild, parent);
    }

    virtual void OnChildMoved(GameObject *parent,
                              GameObject *movedChild,
                              int oldIndex,
                              int newIndex)
    {
        BANG_UNUSED_4(parent, movedChild, oldIndex, newIndex);
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
}

#endif  // IEVENTSCHILDREN_H
