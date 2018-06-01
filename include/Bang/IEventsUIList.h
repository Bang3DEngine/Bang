#ifndef IEVENTSUILIST_H
#define IEVENTSUILIST_H

#include "Bang/IEvents.h"

NAMESPACE_BANG_BEGIN

FORWARD class GameObject;

class IEventsUIList
{
    IEVENTS(IEventsUIList);

public:
    virtual void OnItemAdded(GameObject *item, int index)
    {
        (void) item;
        (void) index;
    }

    virtual void OnItemMoved(GameObject *item, int oldIndex, int newIndex)
    {
        (void) item;
        (void) oldIndex;
        (void) newIndex;
    }

    virtual void OnItemRemoved(GameObject *item)
    {
        (void) item;
    }
};

NAMESPACE_BANG_END

#endif // IEVENTSUILIST_H

