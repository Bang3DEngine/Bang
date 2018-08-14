#ifndef IEVENTSOBJECT_H
#define IEVENTSOBJECT_H

#include "Bang/IEvents.h"

NAMESPACE_BANG_BEGIN

FORWARD class Object;

class IObjectEvents
{
    IEVENTS(IObjectEvents);

public:
    virtual void OnStarted(Object *obj)
    {
    }

    virtual void OnEnabled(Object *obj)
    {
    }

    virtual void OnDisabled(Object *obj)
    {
    }
};

NAMESPACE_BANG_END

#endif // IEVENTSOBJECT_H
