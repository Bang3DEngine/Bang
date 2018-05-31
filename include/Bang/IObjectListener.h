#ifndef IOBJECTLISTENER_H
#define IOBJECTLISTENER_H

#include "Bang/Bang.h"

NAMESPACE_BANG_BEGIN

class IObjectEvents
{
public:
    virtual void OnStarted()  {}
    virtual void OnEnabled()  {}
    virtual void OnDisabled() {}
};

NAMESPACE_BANG_END

#endif // IOBJECTLISTENER_H
