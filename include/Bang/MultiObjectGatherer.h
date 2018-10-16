#ifndef MULTIOBJECTGATHERER_H
#define MULTIOBJECTGATHERER_H

#include "Bang/Bang.h"
#include "Bang/USet.h"
#include "Bang/TypeMap.h"
#include "Bang/EventEmitter.h"
#include "Bang/EventListener.h"
#include "Bang/IEventsDestroy.h"
#include "Bang/ObjectGatherer.h"
#include "Bang/IEventsChildren.h"
#include "Bang/IEventsComponent.h"
#include "Bang/IEventsObjectGatherer.h"

NAMESPACE_BANG_BEGIN

FORWARD class Object;
FORWARD class GameObject;

template <class ObjectType, bool RECURSIVE>
class MultiObjectGatherer : public EventListener<IEventsDestroy>
{
public:
    using ObjectGathererT = ObjectGatherer<ObjectType, RECURSIVE>;

    MultiObjectGatherer()  = default;
    virtual ~MultiObjectGatherer() override;

    const Array<ObjectType*>& GetGatheredArray(GameObject *go) const;

private:
    mutable Map<GameObject*, ObjectGathererT*> m_objectGatherers;

    // IEventsDestroy
    virtual void OnDestroyed(EventEmitter<IEventsDestroy> *destroyedObj) override;
};

NAMESPACE_BANG_END

#include "Bang/MultiObjectGatherer.tcc"

#endif // MULTIOBJECTGATHERER_H

