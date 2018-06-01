#ifndef IEVENTSDEBUG_H
#define IEVENTSDEBUG_H

#include "Bang/IEvents.h"
#include "Bang/DebugMessageType.h"

NAMESPACE_BANG_BEGIN

FORWARD class String;

class IEventsDebug
{
    IEVENTS(IEventsDebug);

public:
    virtual void OnMessage(DebugMessageType msgType,
                           const String &str,
                           int line,
                           const String &fileName) = 0;
};

NAMESPACE_BANG_END

#endif // IEVENTSDEBUG_H
