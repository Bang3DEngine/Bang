#ifndef PXSCENECONTAINER_H
#define PXSCENECONTAINER_H

#include "PxPhysicsAPI.h"

#include "Bang/Map.h"
#include "Bang/Time.h"
#include "Bang/RayCastInfo.h"
#include "Bang/EventEmitter.h"
#include "Bang/EventListener.h"
#include "Bang/PhysicsObject.h"
#include "Bang/ObjectGatherer.h"
#include "Bang/RayCastHitInfo.h"
#include "Bang/IEventsChildren.h"

NAMESPACE_BANG_BEGIN

class PxSceneContainer : public EventListener<IEventsObjectGatherer<PhysicsObject>>,
                         public EventListener<IEventsDestroy>,
                         public physx::PxSimulationEventCallback
{
public:
    PxSceneContainer(Scene *scene);
    virtual ~PxSceneContainer();

    Time::TimeT m_lastStepTimeMillis = 0;

    int m_numFramesLeftToIgnore = 0;
    physx::PxScene *p_pxScene = nullptr;
    ObjectGatherer<PhysicsObject, true> *m_physicsObjectGatherer = nullptr;
    Map<GameObject*, physx::PxActor*> m_gameObjectToPxActor;
    Map<physx::PxActor*, GameObject*> m_pxActorToGameObject;
    Map<physx::PxShape*, Collider*> m_pxShapeToCollider;

    void ResetStepTimeReference();

    physx::PxScene* GetPxScene() const;
    physx::PxActor* GetPxActorFromGameObject(GameObject *go) const;
    Collider* GetColliderFromPxShape(physx::PxShape *pxShape) const;
    GameObject* GetGameObjectFromPxActor(physx::PxActor *pxActor) const;

    void RayCast(const RayCastInfo &rcInfo, RayCastHitInfo *hitInfo);

    physx::PxActor* GetAncestorOrThisPxActor(GameObject *go);
    void SynchronizePxActorCreationReleasingWithPhysX(GameObject *go);

    // PxSimulationEventCallback
    void onConstraintBreak(physx::PxConstraintInfo* constraints,
                           physx::PxU32 count) override;
    void onWake(physx::PxActor** actors, physx::PxU32 count) override;
    void onSleep(physx::PxActor** actors, physx::PxU32 count) override;
    void onContact(const physx::PxContactPairHeader& pairHeader,
                   const physx::PxContactPair* pairs,
                   physx::PxU32 nbPairs) override;
    void onTrigger(physx::PxTriggerPair* pairs,
                   physx::PxU32 count) override;
    void onAdvance(const physx::PxRigidBody * const * bodyBuffer,
                   const physx::PxTransform* poseBuffer,
                   const physx::PxU32 count) override;

    // IEventsObjectGatherer
    virtual void OnObjectGathered(PhysicsObject *phObj) override;
    virtual void OnObjectUnGathered(GameObject *previousGameObject,
                                    PhysicsObject *phObj) override;

    // IEventsDestroy
    virtual void OnDestroyed(EventEmitter<IEventsDestroy> *ee) override;
};

NAMESPACE_BANG_END

#endif // PXSCENECONTAINER_H

