#ifndef IEVENTSCOMPONENT_H
#define IEVENTSCOMPONENT_H

#include "Bang/IEvents.h"

NAMESPACE_BANG_BEGIN

FORWARD class Component;

class IEventsComponent
{
    IEVENTS(IEventsComponent);

public:
    virtual void OnComponentAdded(Component *addedComponent, int index)
    {
        (void) addedComponent;
        (void) index;
    }

    virtual void OnComponentRemoved(Component *removedComponent)
    {
        (void) removedComponent;
    }
};

NAMESPACE_BANG_END

#endif // IEVENTSCOMPONENT_H
