#ifndef IRENDERERCHANGEDLISTENER_H
#define IRENDERERCHANGEDLISTENER_H

#include "Bang/EventListener.h"

NAMESPACE_BANG_BEGIN

FORWARD class Renderer;

class IRendererChangedListener
{
    EVENTLISTENER(IRendererChangedListener);

public:
    virtual void OnRendererChanged(Renderer *changedRenderer) = 0;
};

NAMESPACE_BANG_END

#endif // IRENDERERCHANGEDLISTENER_H
