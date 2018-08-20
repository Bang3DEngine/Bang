#ifndef IEVENTSCOMPONENTCHANGEGAMEOBJECT_H
#define IEVENTSCOMPONENTCHANGEGAMEOBJECT_H

#include "Bang/IEvents.h"

NAMESPACE_BANG_BEGIN

FORWARD class Component;

class IEventsComponentChangeGameObject
{
    IEVENTS(IEventsComponentChangeGameObject)

public:
    virtual void OnComponentChangedGameObject(GameObject *previousGameObject,
                                              GameObject *newGameObject,
                                              Component *component) = 0;
};

NAMESPACE_BANG_END

#endif // IEVENTSCOMPONENT_H
