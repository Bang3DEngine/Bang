#ifndef IEVENTSOBJECTGATHERER_H
#define IEVENTSOBJECTGATHERER_H

#include "Bang/IEvents.h"

namespace Bang
{
template <class ObjectType>
class IEventsObjectGatherer
{
    IEVENTS(IEventsObjectGatherer);

public:
    virtual void OnObjectGathered(ObjectType *obj)
    {
        BANG_UNUSED(obj);
    }

    virtual void OnObjectUnGathered(GameObject *previousGameObject,
                                    ObjectType *obj)
    {
        BANG_UNUSED_2(obj, previousGameObject);
    }
};
}

#endif  // IEVENTSOBJECTGATHERER_H
