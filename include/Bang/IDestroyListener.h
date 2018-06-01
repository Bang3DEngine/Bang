#ifndef IDESTROYLISTENER_H
#define IDESTROYLISTENER_H

#include "Bang/EventEmitter.h"
#include "Bang/EventListener.h"

NAMESPACE_BANG_BEGIN

class IDestroyListener
{
    EVENTLISTENER(IDestroyListener);

public:
    virtual void OnDestroyed(EventEmitter<IDestroyListener> *object) = 0;
};

NAMESPACE_BANG_END

#endif // IDESTROYLISTENER_H

