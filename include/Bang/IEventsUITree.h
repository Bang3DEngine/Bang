#ifndef IEVENTSUITREE_H
#define IEVENTSUITREE_H

#include "Bang/IEvents.h"

namespace Bang
{
class GameObject;
class UIDragDroppable;

class IEventsUITree
{
    IEVENTS(IEventsUITree);

public:
    virtual void OnItemAdded(GameObject *item,
                             GameObject *parentItem,
                             int indexInsideParent)
    {
        BANG_UNUSED_3(item, parentItem, indexInsideParent);
    }

    virtual void OnItemMoved(GameObject *item,
                             GameObject *oldParentItem,
                             int oldIndexInsideParent,
                             GameObject *newParentItem,
                             int newIndexInsideParent)
    {
        BANG_UNUSED_5(item,
                      oldParentItem,
                      oldIndexInsideParent,
                      newParentItem,
                      newIndexInsideParent);
    }

    virtual void OnItemRemoved(GameObject *item)
    {
        BANG_UNUSED(item);
    }

    virtual void OnDropOutside(UIDragDroppable *dropped)
    {
        BANG_UNUSED(dropped);
    }

    virtual void OnDropFromOutside(UIDragDroppable *dropped,
                                   GameObject *newParentItem,
                                   int newIndexInsideParent)
    {
        BANG_UNUSED_3(dropped, newParentItem, newIndexInsideParent);
    }

    virtual bool AcceptDragOrDrop(UIDragDroppable *dd)
    {
        BANG_UNUSED(dd);
        return true;
    }
};
}

#endif  // IEVENTSUITREE_H
