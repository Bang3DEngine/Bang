#ifndef IEVENTSVALUECHANGED_H
#define IEVENTSVALUECHANGED_H

#include "Bang/IEvents.h"

NAMESPACE_BANG_BEGIN

class IEventsValueChanged
{
    IEVENTS(IEventsValueChanged);

public:
    virtual void OnValueChanged(EventEmitter<IEventsValueChanged> *ee) = 0;
};

NAMESPACE_BANG_END

#endif // IEVENTSVALUECHANGED_H
