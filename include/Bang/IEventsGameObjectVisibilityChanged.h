#ifndef IEVENTSGAMEOBJECTVISIBILITYCHANGED_H
#define IEVENTSGAMEOBJECTVISIBILITYCHANGED_H

#include "Bang/IEvents.h"

namespace Bang
{
class GameObject;

class IEventsGameObjectVisibilityChanged
{
    IEVENTS(IEventsGameObjectVisibilityChanged);

public:
    virtual void OnVisibilityChanged(GameObject *go) = 0;
};
}

#endif  // IEVENTSGAMEOBJECTVISIBILITYCHANGED_H
