#ifndef ITEXTURECHANGEDLISTENER_H
#define ITEXTURECHANGEDLISTENER_H

#include "Bang/Bang.h"

NAMESPACE_BANG_BEGIN

FORWARD class Texture;

class ITextureChangedListener
{
public:
    virtual ~ITextureChangedListener() = default;
    virtual void OnTextureChanged(const Texture *changedTexture) = 0;
};

NAMESPACE_BANG_END

#endif // ITEXTURECHANGEDLISTENER_H
