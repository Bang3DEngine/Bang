#ifndef IEVENTSUILIST_H
#define IEVENTSUILIST_H

#include "Bang/IEvents.h"

namespace Bang
{
class GameObject;
class UIDragDroppable;

class IEventsUIList
{
    IEVENTS(IEventsUIList);

public:
    virtual void OnItemAdded(GameObject *item, int index)
    {
        BANG_UNUSED_2(item, index);
    }

    virtual void OnItemMoved(GameObject *item, int oldIndex, int newIndex)
    {
        BANG_UNUSED_3(item, oldIndex, newIndex);
    }

    virtual void OnItemRemoved(GameObject *item)
    {
        BANG_UNUSED(item);
    }

    virtual void OnDropOutside(UIDragDroppable *dd)
    {
        BANG_UNUSED(dd);
    }
};
}

#endif  // IEVENTSUILIST_H
