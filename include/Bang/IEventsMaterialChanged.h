#ifndef IEVENTSMATERIALCHANGED_H
#define IEVENTSMATERIALCHANGED_H

#include "Bang/IEvents.h"

NAMESPACE_BANG_BEGIN

FORWARD class Material;

class IEventsMaterialChanged
{
    IEVENTS(IEventsMaterialChanged);

public:
    virtual void OnMaterialChanged(Material *changedMaterial) = 0;
};

NAMESPACE_BANG_END

#endif // IEVENTSMATERIALCHANGED_H
