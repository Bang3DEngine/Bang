#ifndef IMATERIALCHANGEDLISTENER_H
#define IMATERIALCHANGEDLISTENER_H

#include "Bang/EventListener.h"

NAMESPACE_BANG_BEGIN

FORWARD class Material;

class IMaterialChangedListener
{
    EVENTLISTENER(IMaterialChangedListener);

public:
    virtual void OnMaterialChanged(Material *changedMaterial) = 0;
};

NAMESPACE_BANG_END

#endif // IMATERIALCHANGEDLISTENER_H
