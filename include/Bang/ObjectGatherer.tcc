#pragma once

#include "Bang/Component.h"
#include "Bang/GameObject.h"
#include "Bang/ObjectGatherer.h"

namespace Bang
{
template <class ObjectType, bool RECURSIVE>
ObjectGatherer<ObjectType, RECURSIVE>::~ObjectGatherer()
{
}

template <class ObjectType, bool RECURSIVE>
void ObjectGatherer<ObjectType, RECURSIVE>::SetRoot(GameObject *root)
{
    if (root != GetRoot())
    {
        if (GetRoot())
        {
            UnRegisterEventsAndRemoveObjects(GetRoot());
        }
        ASSERT(m_gatheredObjects.IsEmpty());

#ifdef DEBUG
        ASSERT(m_processedGameObjects.IsEmpty());
#endif

        p_root = root;
        m_gatheredObjects.Clear();

#ifdef DEBUG
        m_processedGameObjects.Clear();
#endif

        if (GetRoot())
        {
            RegisterEventsAndGather(GetRoot());
        }
    }
}

template <class ObjectType, bool RECURSIVE>
GameObject *ObjectGatherer<ObjectType, RECURSIVE>::GetRoot() const
{
    return p_root;
}

template <class ObjectType, bool RECURSIVE>
const Array<ObjectType *>
    &ObjectGatherer<ObjectType, RECURSIVE>::GetGatheredObjects() const
{
    return m_gatheredObjects;
}

template <class ObjectType, bool RECURSIVE>
void ObjectGatherer<ObjectType, RECURSIVE>::RegisterEventsAndGather(
    GameObject *go)
{
#ifdef DEBUG
    ASSERT(!m_processedGameObjects.Contains(go))
    m_processedGameObjects.Add(go);
#endif

    go->EventEmitter<IEventsDestroy>::RegisterListener(this);
    go->EventEmitter<IEventsComponent>::RegisterListener(this);

    Array<ObjectType *> goObjectsOfTheType;
    GatherObjectsOfTheType(&goObjectsOfTheType, go);
    for (ObjectType *obj : goObjectsOfTheType)
    {
        AddGatheredObject(obj);
    }

    if (RECURSIVE)
    {
        go->EventEmitter<IEventsChildren>::RegisterListener(this);
        for (GameObject *child : go->GetChildren())
        {
            RegisterEventsAndGather(child);
        }
    }
}

template <class ObjectType, bool RECURSIVE>
void ObjectGatherer<ObjectType, RECURSIVE>::UnRegisterEventsAndRemoveObjects(
    GameObject *go)
{
#ifdef DEBUG
    if (RECURSIVE)
    {
        // ASSERT(m_processedGameObjects.Contains(go));
    }
    else
    {
        ASSERT(go == GetRoot());
    }
    m_processedGameObjects.Remove(go);
#endif

    go->EventEmitter<IEventsDestroy>::UnRegisterListener(this);
    go->EventEmitter<IEventsComponent>::UnRegisterListener(this);

    Array<ObjectType *> goObjectsOfTheType;
    GatherObjectsOfTheType(&goObjectsOfTheType, go);
    for (ObjectType *obj : goObjectsOfTheType)
    {
        RemoveGatheredObject(obj, go);
    }

    if (RECURSIVE)
    {
        go->EventEmitter<IEventsChildren>::UnRegisterListener(this);
        for (GameObject *child : go->GetChildren())
        {
            UnRegisterEventsAndRemoveObjects(child);
        }
    }
}

template <class ObjectType, bool RECURSIVE>
void ObjectGatherer<ObjectType, RECURSIVE>::GatherObjectsOfTheType(
    Array<ObjectType *> *gatheredObjects,
    GameObject *go)
{
    if (ObjectType *obj = DCAST<ObjectType *>(go))
    {
        gatheredObjects->PushBack(obj);
    }

    for (Component *comp : go->GetComponents())
    {
        if (ObjectType *obj = DCAST<ObjectType *>(comp))
        {
            gatheredObjects->PushBack(obj);
        }
    }
}

template <class ObjectType, bool RECURSIVE>
void ObjectGatherer<ObjectType, RECURSIVE>::OnChildAdded(GameObject *addedChild,
                                                         GameObject *)
{
    ASSERT(RECURSIVE);
    RegisterEventsAndGather(addedChild);
}

template <class ObjectType, bool RECURSIVE>
void ObjectGatherer<ObjectType, RECURSIVE>::OnChildRemoved(
    GameObject *removedChild,
    GameObject *)
{
    ASSERT(RECURSIVE);
    UnRegisterEventsAndRemoveObjects(removedChild);
}

template <class ObjectType, bool RECURSIVE>
void ObjectGatherer<ObjectType, RECURSIVE>::OnComponentAdded(
    Component *addedComponent,
    int)
{
    if (ObjectType *obj = DCAST<ObjectType *>(addedComponent))
    {
        AddGatheredObject(obj);
    }
}

template <class ObjectType, bool RECURSIVE>
void ObjectGatherer<ObjectType, RECURSIVE>::OnComponentRemoved(
    Component *removedComponent,
    GameObject *previousGameObject)
{
    if (ObjectType *obj = DCAST<ObjectType *>(removedComponent))
    {
        RemoveGatheredObject(obj, previousGameObject);
    }
}

template <class ObjectType, bool RECURSIVE>
void ObjectGatherer<ObjectType, RECURSIVE>::AddGatheredObject(ObjectType *obj)
{
    m_gatheredObjects.PushBack(obj);
    EventEmitter<IEventsObjectGatherer<ObjectType>>::PropagateToListeners(
        &IEventsObjectGatherer<ObjectType>::OnObjectGathered, obj);
}

template <class ObjectType, bool RECURSIVE>
void ObjectGatherer<ObjectType, RECURSIVE>::RemoveGatheredObject(
    ObjectType *obj,
    GameObject *previousGameObject)
{
    m_gatheredObjects.Remove(obj);
    EventEmitter<IEventsObjectGatherer<ObjectType>>::PropagateToListeners(
        &IEventsObjectGatherer<ObjectType>::OnObjectUnGathered,
        previousGameObject,
        obj);
}

template <class ObjectType, bool RECURSIVE>
void ObjectGatherer<ObjectType, RECURSIVE>::ObjectGatherer::OnDestroyed(
    EventEmitter<IEventsDestroy> *object)
{
    GameObject *go = DCAST<GameObject *>(object);

    Array<GameObject *> gosToUnRegister;
    if (RECURSIVE)
    {
        Array<GameObject *> childrenRecursive;
        for (GameObject *child : childrenRecursive)
        {
            gosToUnRegister.PushBack(child);
        }
    }

    gosToUnRegister.PushBack(go);
    for (GameObject *childOrGo : gosToUnRegister)
    {
        UnRegisterEventsAndRemoveObjects(childOrGo);
    }

    if (go == GetRoot())
    {
        SetRoot(nullptr);
    }
}
}
