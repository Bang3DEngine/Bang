#ifndef IEVENTSCOMPONENT_H
#define IEVENTSCOMPONENT_H

#include "Bang/IEvents.h"

namespace Bang
{
class Component;

class IEventsComponent
{
    IEVENTS(IEventsComponent);

public:
    virtual void OnComponentAdded(Component *addedComponent, int index)
    {
        BANG_UNUSED_2(addedComponent, index);
    }

    virtual void OnComponentRemoved(Component *removedComponent,
                                    GameObject *previousGameObject)
    {
        BANG_UNUSED_2(removedComponent, previousGameObject);
    }
};
}

#endif  // IEVENTSCOMPONENT_H
