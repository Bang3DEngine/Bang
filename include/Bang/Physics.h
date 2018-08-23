#ifndef PHYSICS_H
#define PHYSICS_H

#include "PxPhysicsAPI.h"

#include "Bang/Map.h"
#include "Bang/EventEmitter.h"
#include "Bang/EventListener.h"
#include "Bang/ObjectGatherer.h"
#include "Bang/IEventsChildren.h"
#include "Bang/IEventsComponentChangeGameObject.h"

NAMESPACE_BANG_BEGIN

FORWARD class Scene;
FORWARD class RigidBody;
FORWARD class PhysicsObject;
FORWARD class PxSceneContainer;

class Physics : public EventListener<IEventsDestroy>
{
public:
	Physics();
	virtual ~Physics();

    void Init();

    void Step(Scene *scene, float simulationTime);
    void StepIfNeeded(Scene *scene);
    void ResetStepTimeReference(Scene *scene);
    void UpdateFromTransforms(Scene *scene);
    void SetIgnoreNextFrames(Scene *scene, int numNextFramesToIgnore);

    void RegisterScene(Scene *scene);
    void UpdateRigidbodyValues(RigidBody *rb);

    void SetStepSleepTime(float stepSleepTimeSeconds);
    void SetMaxSubSteps(int maxSubSteps);
    void SetGravity(const Vector3 &gravity);

    int GetMaxSubSteps() const;
    float GetStepSleepTimeSeconds() const;
    const Vector3& GetGravity() const;

    static Vector2 GetVector2FromPxVec2(const physx::PxVec2 &v);
    static Vector3 GetVector3FromPxVec3(const physx::PxVec3 &v);
    static Vector4 GetVector4FromPxVec4(const physx::PxVec4 &v);
    static Quaternion GetQuaternionFromPxQuat(const physx::PxQuat &q);
    static physx::PxVec2 GetPxVec2FromVector2(const Vector2 &v);
    static physx::PxVec3 GetPxVec3FromVector3(const Vector3 &v);
    static physx::PxVec4 GetPxVec4FromVector4(const Vector4 &v);
    static physx::PxQuat GetPxQuatFromQuaternion(const Quaternion &q);
    static Physics *GetInstance();

    // IEventsDestroy
    virtual void OnDestroyed(EventEmitter<IEventsDestroy> *ee) override;

private:
    physx::PxDefaultAllocator m_pxAllocator;
    physx::PxDefaultErrorCallback m_pxErrorCallback;
    physx::PxFoundation *m_pxFoundation = nullptr;
    physx::PxPhysics *m_pxPhysics = nullptr;

    int m_maxSubSteps = 10;
    float m_stepSleepTimeSeconds = (1.0f/60.0f);
    Vector3 m_gravity = Vector3(0.0f, -9.81f, 0.0f);

    Map<Scene*, PxSceneContainer*> m_sceneToPxSceneContainer;

    Scene* GetSceneFromPhysicsObject(PhysicsObject *phObj) const;
    PxSceneContainer* GetPxSceneContainerFromScene(Scene *scene);
    const PxSceneContainer* GetPxSceneContainerFromScene(Scene *scene) const;

    physx::PxFoundation* GetPxFoundation() const;
    physx::PxPhysics* GetPxPhysics() const;

    physx::PxMaterial* CreateNewMaterial();
    physx::PxActor* CreateIntoPxScene(PhysicsObject *phObj);

    static GameObject* GetGameObjectFromPhysicsObject(PhysicsObject *phObj);

    static void FillTransformFromPxTransform(Transform *transform,
                                             const physx::PxTransform &pxTransform);
    static physx::PxTransform GetPxTransformFromTransform(Transform *tr);

    friend class PxSceneContainer;
};

class PxSceneContainer : public EventListener<IEventsObjectGatherer<PhysicsObject>>,
                         public EventListener<IEventsDestroy>
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

    void ResetStepTimeReference();

    physx::PxScene* GetPxScene() const;
    physx::PxActor* GetPxActorFromGameObject(GameObject *go) const;
    GameObject* GetGameObjectFromPxActor(physx::PxActor *pxActor) const;

    void ReleasePxActorIfNoMorePhysicsObjectsOnIt(GameObject *go);

    // IEventsObjectGatherer
    virtual void OnObjectGathered(PhysicsObject *phObj) override;
    virtual void OnObjectUnGathered(GameObject *previousGameObject,
                                    PhysicsObject *phObj) override;

    // IEventsDestroy
    virtual void OnDestroyed(EventEmitter<IEventsDestroy> *ee) override;
};

NAMESPACE_BANG_END

#endif // PHYSICS_H

