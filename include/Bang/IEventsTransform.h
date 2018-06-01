#ifndef IEVENTSTRANSFORM_H
#define IEVENTSTRANSFORM_H

#include "Bang/IEvents.h"

NAMESPACE_BANG_BEGIN

class IEventsTransform
{
    IEVENTS(IEventsTransform);

public:
    virtual void OnTransformChanged()
    {
    }

    virtual void OnParentTransformChanged()
    {
    }

    virtual void OnChildrenTransformChanged()
    {
    }
};

NAMESPACE_BANG_END

#endif // IEVENTSTRANSFORM_H
