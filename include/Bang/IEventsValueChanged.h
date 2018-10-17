#ifndef IEVENTSVALUECHANGED_H
#define IEVENTSVALUECHANGED_H

#include "Bang/IEvents.h"

namespace Bang
{
class IEventsValueChanged
{
    IEVENTS(IEventsValueChanged);

public:
    virtual void OnValueChanged(EventEmitter<IEventsValueChanged> *ee) = 0;
};
}

#endif  // IEVENTSVALUECHANGED_H
