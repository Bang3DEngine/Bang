#ifndef IEVENTSNAME_H
#define IEVENTSNAME_H

#include "Bang/IEvents.h"

namespace Bang
{
class String;
class GameObject;

class IEventsName
{
    IEVENTS(IEventsName);

public:
    virtual void OnNameChanged(GameObject *go,
                               const String &oldName,
                               const String &newName) = 0;
};
}

#endif  // IEVENTSNAME_H
