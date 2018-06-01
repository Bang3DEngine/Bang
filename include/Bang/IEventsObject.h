#ifndef IEVENTSOBJECT_H
#define IEVENTSOBJECT_H

#include "Bang/IEvents.h"

NAMESPACE_BANG_BEGIN

class IObjectEvents
{
    IEVENTS(IObjectEvents);

public:
    virtual void OnStarted()
    {
    }

    virtual void OnEnabled()
    {
    }

    virtual void OnDisabled()
    {
    }
};

NAMESPACE_BANG_END

#endif // IEVENTSOBJECT_H
