#ifndef IMATERIALCHANGEDLISTENER_H
#define IMATERIALCHANGEDLISTENER_H

#include "Bang/Bang.h"

NAMESPACE_BANG_BEGIN

FORWARD class Material;

class IMaterialChangedListener
{
public:
    virtual void OnMaterialChanged(Material *changedMaterial) = 0;
};

NAMESPACE_BANG_END

#endif // IMATERIALCHANGEDLISTENER_H
