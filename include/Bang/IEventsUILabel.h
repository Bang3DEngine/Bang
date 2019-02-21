#ifndef IEVENTSUILABEL_H
#define IEVENTSUILABEL_H

#include "Bang/IEvents.h"
#include "Bang/String.h"

namespace Bang
{
class IEventsUILabel
{
    IEVENTS(IEventsUILabel);

public:
    virtual void OnFloatingInputTextCommited(const String &commitedText)
    {
        BANG_UNUSED(commitedText);
    }
};
}

#endif  // IEVENTSUILABEL_H
