#ifndef IEVENTSNAME_H
#define IEVENTSNAME_H

#include "Bang/IEvents.h"

NAMESPACE_BANG_BEGIN

FORWARD class String;
FORWARD class GameObject;

class IEventsName
{
    IEVENTS(IEventsName);

public:
    virtual void OnNameChanged(GameObject *go,
                               const String &oldName,
                               const String &newName) = 0;
};

NAMESPACE_BANG_END

#endif // IEVENTSNAME_H
