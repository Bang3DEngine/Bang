#ifndef ITEXTURECHANGEDLISTENER_H
#define ITEXTURECHANGEDLISTENER_H

#include "Bang/EventListener.h"

NAMESPACE_BANG_BEGIN

FORWARD class Texture;

class ITextureChangedListener
{
    EVENTLISTENER(ITextureChangedListener);

public:
    virtual void OnTextureChanged(const Texture *changedTexture) = 0;
};

NAMESPACE_BANG_END

#endif // ITEXTURECHANGEDLISTENER_H
