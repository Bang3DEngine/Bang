#ifndef IEVENTSGAMEOBJECTVISIBILITYCHANGED_H
#define IEVENTSGAMEOBJECTVISIBILITYCHANGED_H

#include "Bang/IEvents.h"

NAMESPACE_BANG_BEGIN

FORWARD class GameObject;

class IEventsGameObjectVisibilityChanged
{
    IEVENTS(IEventsGameObjectVisibilityChanged);

public:
    virtual void OnVisibilityChanged(GameObject *go) = 0;
};

NAMESPACE_BANG_END

#endif // IEVENTSGAMEOBJECTVISIBILITYCHANGED_H
