#ifndef IRENDERERCHANGEDLISTENER_H
#define IRENDERERCHANGEDLISTENER_H

#include "Bang/Bang.h"

NAMESPACE_BANG_BEGIN

FORWARD class Renderer;

class IRendererChangedListener
{
public:
    virtual void OnRendererChanged(Renderer *changedRenderer) = 0;
};

NAMESPACE_BANG_END

#endif // IRENDERERCHANGEDLISTENER_H
