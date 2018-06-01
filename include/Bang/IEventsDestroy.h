#ifndef IEVENTSDESTROY_H
#define IEVENTSDESTROY_H

#include "Bang/IEvents.h"

NAMESPACE_BANG_BEGIN

class IEventsDestroy
{
    IEVENTS(IEventsDestroy);

public:
    virtual void OnDestroyed(EventEmitter<IEventsDestroy> *object) = 0;
};

NAMESPACE_BANG_END

#endif // IEVENTSDESTROY_H

