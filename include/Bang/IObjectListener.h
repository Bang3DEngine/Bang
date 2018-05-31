#ifndef IOBJECTLISTENER_H
#define IOBJECTLISTENER_H

#include "Bang/EventListener.h"

NAMESPACE_BANG_BEGIN

class IObjectEvents
{
    EVENTLISTENER(IObjectEvents);

public:
    virtual void OnStarted()  {}
    virtual void OnEnabled()  {}
    virtual void OnDisabled() {}
};

NAMESPACE_BANG_END

#endif // IOBJECTLISTENER_H
