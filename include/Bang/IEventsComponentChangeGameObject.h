#ifndef IEVENTSCOMPONENTCHANGEGAMEOBJECT_H
#define IEVENTSCOMPONENTCHANGEGAMEOBJECT_H

#include "Bang/IEvents.h"

namespace Bang
{
class Component;

class IEventsComponentChangeGameObject
{
    IEVENTS(IEventsComponentChangeGameObject);

public:
    virtual void OnComponentChangedGameObject(GameObject *previousGameObject,
                                              GameObject *newGameObject,
                                              Component *component) = 0;
};
}

#endif  // IEVENTSCOMPONENT_H
