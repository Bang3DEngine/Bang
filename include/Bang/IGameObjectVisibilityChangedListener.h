#ifndef IGAMEOBJECTVISIBILITYCHANGEDLISTENER_H
#define IGAMEOBJECTVISIBILITYCHANGEDLISTENER_H

#include "Bang/EventListener.h"

NAMESPACE_BANG_BEGIN

FORWARD class Object;

class IGameObjectVisibilityChangedListener
{
    EVENTLISTENER(IGameObjectVisibilityChangedListener);

public:
    virtual void OnVisibilityChanged(GameObject *go) = 0;
};

NAMESPACE_BANG_END

#endif // IGAMEOBJECTVISIBILITYCHANGEDLISTENER_H
