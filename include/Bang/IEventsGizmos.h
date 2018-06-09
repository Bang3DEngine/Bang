#ifndef IEVENTSGIZMOS_H
#define IEVENTSGIZMOS_H

#include "Bang/IEvents.h"

NAMESPACE_BANG_BEGIN

FORWARD class Renderer;
FORWARD class GameObject;

class IEventsGizmos
{
    IEVENTS(IEventsGizmos);

public:
    virtual void OnBeforeRender(Renderer *renderer, GameObject *selectable)
    {
        (void) renderer;
        (void) selectable;
    }

    virtual void OnAfterRender(Renderer *renderer, GameObject *selectable)
    {
        (void) renderer;
        (void) selectable;
    }
};

NAMESPACE_BANG_END

#endif // IEVENTSGIZMOS_H

