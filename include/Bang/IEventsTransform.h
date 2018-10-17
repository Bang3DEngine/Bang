#ifndef IEVENTSTRANSFORM_H
#define IEVENTSTRANSFORM_H

#include "Bang/IEvents.h"

namespace Bang
{
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
}

#endif  // IEVENTSTRANSFORM_H
