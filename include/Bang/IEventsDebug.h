#ifndef IEVENTSDEBUG_H
#define IEVENTSDEBUG_H

#include "Bang/DebugMessageType.h"
#include "Bang/IEvents.h"

namespace Bang
{
class String;

class IEventsDebug
{
    IEVENTS(IEventsDebug);

public:
    virtual void OnMessage(DebugMessageType msgType,
                           const String &str,
                           int line,
                           const String &fileName) = 0;
};
}

#endif  // IEVENTSDEBUG_H
