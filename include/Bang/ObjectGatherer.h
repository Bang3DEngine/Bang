#ifndef OBJECTGATHERER_H
#define OBJECTGATHERER_H

#include "Bang/Bang.h"
#include "Bang/USet.h"
#include "Bang/EventEmitter.h"
#include "Bang/EventListener.h"
#include "Bang/IEventsDestroy.h"
#include "Bang/IEventsChildren.h"
#include "Bang/IEventsComponent.h"
#include "Bang/IEventsObjectGatherer.h"

NAMESPACE_BANG_BEGIN

FORWARD class Object;
FORWARD class GameObject;

template <class ObjectType, bool RECURSIVE>
class ObjectGatherer : public EventListener<IEventsDestroy>,
                       public EventListener<IEventsChildren>,
                       public EventListener<IEventsComponent>,
                       public EventEmitter< IEventsObjectGatherer<ObjectType> >
{
public:
    ObjectGatherer()  = default;
    virtual ~ObjectGatherer();

    void SetRoot(GameObject *root);

    GameObject* GetRoot() const;
    const Array<ObjectType*>& GetGatheredArray() const;

private:
    GameObject *p_root = nullptr;
    Array<ObjectType*> m_gatheredObjects;

    #ifdef DEBUG
    USet<GameObject*> m_processedGameObjects;
    #endif

    void RegisterEventsAndGather(GameObject *go);
    void UnRegisterEventsAndRemoveObjects(GameObject *go);

    static void
    GatherObjectsOfTheType(Array<ObjectType*> *gatheredObjects, GameObject *go);

    // IEventsChildren
    void OnChildAdded(GameObject *addedChild, GameObject *parent) override;
    void OnChildRemoved(GameObject *removedChild, GameObject *parent) override;

    // IEventsComponent
    void OnComponentAdded(Component *addedComponent, int index) override;
    void OnComponentRemoved(Component *removedComponent,
                            GameObject *previousGameObject) override;

    void AddGatheredObject(ObjectType *obj);
    void RemoveGatheredObject(ObjectType *obj, GameObject *previousGameObject);

    // IEventsDestroy
    void OnDestroyed(EventEmitter<IEventsDestroy> *object) override;
};

NAMESPACE_BANG_END

#include "Bang/ObjectGatherer.tcc"

#endif // OBJECTGATHERER_H

