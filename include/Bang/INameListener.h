#ifndef INAMELISTENER_H
#define INAMELISTENER_H

#include "Bang/EventListener.h"

NAMESPACE_BANG_BEGIN

class INameListener
{
    EVENTLISTENER(INameListener);

public:
    virtual void OnNameChanged(GameObject *go, const String &oldName,
                               const String &newName) = 0;
};

NAMESPACE_BANG_END

#endif // ICHILDRENLISTENER_H
