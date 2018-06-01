#ifndef IEVENTSUITREE_H
#define IEVENTSUITREE_H

#include "Bang/IEvents.h"

NAMESPACE_BANG_BEGIN

FORWARD class GameObject;

class IEventsUITree
{
    IEVENTS(IEventsUITree);

public:
    virtual void OnItemAdded(GameObject *item,
                             GameObject *parentItem,
                             int indexInsideParent)
    {
        (void) item;
        (void) parentItem;
        (void) indexInsideParent;
    }

    virtual void OnItemMoved(GameObject *item,
                             GameObject *oldParentItem,
                             int oldIndexInsideParent,
                             GameObject *newParentItem,
                             int newIndexInsideParent)
    {
        (void) item;
        (void) oldParentItem;
        (void) oldIndexInsideParent;
        (void) newParentItem;
        (void) newIndexInsideParent;
    }

    virtual void OnItemRemoved(GameObject *item)
    {
        (void) item;
    }
};

NAMESPACE_BANG_END

#endif // IEVENTSUITREE_H

