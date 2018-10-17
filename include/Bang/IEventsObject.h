#ifndef IEVENTSOBJECT_H
#define IEVENTSOBJECT_H

#include "Bang/IEvents.h"

namespace Bang
{
class Object;

class IEventsObject
{
    IEVENTS(IEventsObject);

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
}

#endif  // IEVENTSOBJECT_H
