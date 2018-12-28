#ifndef PXSCENECONTAINER_H
#define PXSCENECONTAINER_H

#include <functional>
#include <vector>

#include "Bang/Array.h"
#include "Bang/Array.tcc"
#include "Bang/BangDefines.h"
#include "Bang/EventEmitter.tcc"
#include "Bang/EventListener.h"
#include "Bang/IEventsDestroy.h"
#include "Bang/IEventsObjectGatherer.h"
#include "Bang/Map.h"
#include "Bang/Time.h"
#include "PxSimulationEventCallback.h"
#include "foundation/Px.h"

namespace physx
{
class PxActor;
class PxRigidBody;
class PxScene;
class PxShape;
class PxTransform;
}

namespace Bang
{
template <class>
class EventEmitter;
template <class>
class IEventsObjectGatherer;
class Collider;
class GameObject;
class IEventsDestroy;
class PhysicsComponent;
class Scene;
struct RayCastHitInfo;
struct RayCastInfo;
template <class ObjectType, bool RECURSIVE>
class ObjectGatherer;

class PxSceneContainer
    : public EventListener<IEventsObjectGatherer<PhysicsComponent>>,
      public EventListener<IEventsDestroy>,
      public physx::PxSimulationEventCallback
{
public:
    PxSceneContainer(Scene *scene);
    virtual ~PxSceneContainer() override;

    void ResetStepTimeReference();

    static void ChangePxRigidActor(PxSceneContainer *pxSceneContainer,
                                   PhysicsComponent *phComp,
                                   physx::PxRigidActor *newPxRigidActor);

    Scene *GetScene() const;
    physx::PxScene *GetPxScene() const;
    Array<Collider *> GetColliders() const;
    physx::PxActor *GetPxActorFromGameObject(GameObject *go) const;
    Collider *GetColliderFromPxShape(physx::PxShape *pxShape) const;
    GameObject *GetGameObjectFromPxActor(physx::PxActor *pxActor) const;

    void RayCast(const RayCastInfo &rcInfo, RayCastHitInfo *hitInfo);

    physx::PxActor *GetAncestorOrThisPxActor(GameObject *go);

private:
    Time m_lastStepTime;

    Scene *p_scene = nullptr;
    int m_numFramesLeftToIgnore = 0;
    physx::PxScene *p_pxScene = nullptr;
    ObjectGatherer<PhysicsComponent, true> *m_physicsObjectGatherer = nullptr;
    Map<physx::PxShape *, Collider *> m_pxShapeToCollider;
    mutable Map<GameObject *, physx::PxActor *> m_gameObjectToPxActor;
    mutable Map<physx::PxActor *, GameObject *> m_pxActorToGameObject;

    // PxSimulationEventCallback
    void onConstraintBreak(physx::PxConstraintInfo *constraints,
                           physx::PxU32 count) override;
    void onWake(physx::PxActor **actors, physx::PxU32 count) override;
    void onSleep(physx::PxActor **actors, physx::PxU32 count) override;
    void onContact(const physx::PxContactPairHeader &pairHeader,
                   const physx::PxContactPair *pairs,
                   physx::PxU32 nbPairs) override;
    void onTrigger(physx::PxTriggerPair *pairs, physx::PxU32 count) override;
    void onAdvance(const physx::PxRigidBody *const *bodyBuffer,
                   const physx::PxTransform *poseBuffer,
                   const physx::PxU32 count) override;

    // IEventsObjectGatherer
    virtual void OnObjectGathered(PhysicsComponent *phComp) override;
    virtual void OnObjectUnGathered(GameObject *previousGameObject,
                                    PhysicsComponent *phComp) override;

    // IEventsDestroy
    virtual void OnDestroyed(EventEmitter<IEventsDestroy> *ee) override;

    friend class Physics;
};
}

#endif  // PXSCENECONTAINER_H
