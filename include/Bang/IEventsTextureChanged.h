#ifndef IEVENTSTEXTURECHANGED_H
#define IEVENTSTEXTURECHANGED_H

#include "Bang/IEvents.h"

NAMESPACE_BANG_BEGIN

FORWARD class Texture;

class IEventsTextureChanged
{
    IEVENTS(IEventsTextureChanged);

public:
    virtual void OnTextureChanged(const Texture *changedTexture) = 0;
};

NAMESPACE_BANG_END

#endif // IEVENTSTEXTURECHANGED_H
