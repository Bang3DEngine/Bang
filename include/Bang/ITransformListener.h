#ifndef ITRANSFORMLISTENER_H
#define ITRANSFORMLISTENER_H

#include "Bang/EventListener.h"

NAMESPACE_BANG_BEGIN

class ITransformListener
{
    EVENTLISTENER(ITransformListener);

public:
    virtual void OnTransformChanged()         {}
    virtual void OnParentTransformChanged()   {}
    virtual void OnChildrenTransformChanged() {}
};

NAMESPACE_BANG_END

#endif // ITRANSFORMLISTENER_H
