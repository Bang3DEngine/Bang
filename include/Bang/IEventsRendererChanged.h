#ifndef IEVENTSRENDERERCHANGED_H
#define IEVENTSRENDERERCHANGED_H

#include "Bang/IEvents.h"

NAMESPACE_BANG_BEGIN

FORWARD class Renderer;

class IEventsRendererChanged
{
    IEVENTS(IEventsRendererChanged);

public:
    virtual void OnRendererChanged(Renderer *changedRenderer) = 0;
};

NAMESPACE_BANG_END

#endif // IEVENTSRENDERERCHANGED_H
