#ifndef IEVENTSDESTROY_H
#define IEVENTSDESTROY_H

#include "Bang/IEvents.h"

namespace Bang
{
class IEventsDestroy
{
    IEVENTS(IEventsDestroy);

public:
    virtual void OnDestroyed(EventEmitter<IEventsDestroy> *object) = 0;
};
}

#endif  // IEVENTSDESTROY_H
