#ifndef PXSCENECONTAINER_H
#define PXSCENECONTAINER_H

#include <functional>
#include <vector>

#include "Bang/Array.h"
#include "Bang/Array.tcc"
#include "Bang/BangDefines.h"
#include "Bang/EventEmitter.h"
#include "Bang/EventEmitter.tcc"
#include "Bang/EventListener.h"
#include "Bang/IEvents.h"
#include "Bang/IEventsChildren.h"
#include "Bang/Map.h"
#include "Bang/ObjectGatherer.h"
#include "Bang/PhysicsObject.h"
#include "Bang/RayCastHitInfo.h"
#include "Bang/RayCastInfo.h"
#include "Bang/Time.h"
#include "PxPhysicsAPI.h"
#include "PxSimulationEventCallback.h"
#include "foundation/Px.h"

FORWARD namespace physx
{
FORWARD class PxActor;
FORWARD class PxRigidBody;
FORWARD class PxScene;
FORWARD class PxShape;
FORWARD class PxTransform;
}

NAMESPACE_BANG_BEGIN

FORWARD   class  Collider;
FORWARD   class  GameObject;
FORWARD   class  IEventsDestroy;
FORWARD   class  PhysicsObject;
FORWARD   class  Scene;
FORWARD   struct RayCastHitInfo;
FORWARD   struct RayCastInfo;
FORWARD_T class  EventEmitter;
FORWARD_T class  IEventsObjectGatherer;
FORWARD   template <class ObjectType, bool RECURSIVE> class ObjectGatherer;

class PxSceneContainer : public EventListener<IEventsObjectGatherer<PhysicsObject>>,
                         public EventListener<IEventsDestroy>,
                         public physx::PxSimulationEventCallback
{
public:
    PxSceneContainer(Scene *scene);
    virtual ~PxSceneContainer() override;

    void ResetStepTimeReference();

    Scene *GetScene() const;
    physx::PxScene* GetPxScene() const;
    Array<Collider*> GetColliders() const;
    physx::PxActor* GetPxActorFromGameObject(GameObject *go) const;
    Collider* GetColliderFromPxShape(physx::PxShape *pxShape) const;
    GameObject* GetGameObjectFromPxActor(physx::PxActor *pxActor) const;

    void RayCast(const RayCastInfo &rcInfo, RayCastHitInfo *hitInfo);

    physx::PxActor* GetAncestorOrThisPxActor(GameObject *go);

private:
    Time m_lastStepTime;

    Scene *p_scene = nullptr;
    int m_numFramesLeftToIgnore = 0;
    physx::PxScene *p_pxScene = nullptr;
    ObjectGatherer<PhysicsObject, true> *m_physicsObjectGatherer = nullptr;
    Map<physx::PxShape*, Collider*> m_pxShapeToCollider;
    mutable Map<GameObject*, physx::PxActor*> m_gameObjectToPxActor;
    mutable Map<physx::PxActor*, GameObject*> m_pxActorToGameObject;

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

    friend class Physics;
};

NAMESPACE_BANG_END

#endif // PXSCENECONTAINER_H

