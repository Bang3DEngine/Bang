#pragma once

#include "Bang/Component.h"
#include "Bang/GameObject.h"
#include "Bang/MultiObjectGatherer.h"

namespace Bang
{
template <class ObjectType, bool RECURSIVE>
MultiObjectGatherer<ObjectType, RECURSIVE>::~MultiObjectGatherer()
{
    for (const auto &it : m_objectGatherers)
    {
        ObjectGathererT *objGatherer = it.second;
        delete objGatherer;
    }
}

template <class ObjectType, bool RECURSIVE>
const Array<ObjectType *>
    &MultiObjectGatherer<ObjectType, RECURSIVE>::GetGatheredArray(
        GameObject *go) const
{
    ObjectGathererT *objGatherer = nullptr;

    auto it = m_objectGatherers.Find(go);
    if (it == m_objectGatherers.End())
    {
        m_objectGatherers.Add(go, new ObjectGathererT());

        objGatherer = m_objectGatherers.Get(go);
        objGatherer->SetRoot(go);

        auto *ncThis =
            const_cast<MultiObjectGatherer<ObjectType, RECURSIVE> *>(this);
        go->EventEmitter<IEventsDestroy>::RegisterListener(ncThis);
    }
    else
    {
        objGatherer = it->second;
    }
    return objGatherer->GetGatheredObjects();
}

template <class ObjectType, bool RECURSIVE>
void MultiObjectGatherer<ObjectType, RECURSIVE>::OnDestroyed(
    EventEmitter<IEventsDestroy> *destroyedObj)
{
    if (GameObject *destroyedGo = DCAST<GameObject *>(destroyedObj))
    {
        auto it = m_objectGatherers.Find(destroyedGo);
        if (it != m_objectGatherers.End())
        {
            ObjectGatherer<ObjectType, RECURSIVE> *objectGatherer = it->second;
            delete objectGatherer;
            m_objectGatherers.Remove(destroyedGo);
        }
    }
}
}
