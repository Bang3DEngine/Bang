#ifndef IEVENTSRENDERERCHANGED_H
#define IEVENTSRENDERERCHANGED_H

#include "Bang/IEvents.h"

namespace Bang
{
class Renderer;

class IEventsRendererChanged
{
    IEVENTS(IEventsRendererChanged);

public:
    virtual void OnRendererChanged(Renderer *changedRenderer) = 0;
};
}

#endif  // IEVENTSRENDERERCHANGED_H
