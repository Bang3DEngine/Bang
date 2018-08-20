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
FORWARD class PxSceneContainer;

class Physics : public EventListener<IEventsComponentChangeGameObject>
{
public:
	Physics();
	virtual ~Physics();

    void Init();

    void Step(Scene *scene, float simulationTime);
    void StepIfNeeded(Scene *scene);
    void ResetStepTimeReference(Scene *scene);
    void UpdateFromTransforms(Scene *scene);

    void RegisterScene(Scene *scene);
    void UnRegisterScene(Scene *scene);
    void RegisterRigidbody(RigidBody *rb);
    void UnRegisterRigidbody(RigidBody *rb);

    void UpdateRigidbodyValues(RigidBody *rb);

    float GetSleepStepTimeSeconds() const;

    static Physics *GetInstance();

private:
    physx::PxDefaultAllocator m_pxAllocator;
    physx::PxDefaultErrorCallback m_pxErrorCallback;
    physx::PxFoundation *m_pxFoundation = nullptr;
    physx::PxPhysics *m_pxPhysics = nullptr;

    float m_sleepStepTimeSeconds = 0.025f;

    Map<Scene*, PxSceneContainer*> m_sceneToPxSceneContainer;

    Scene* GetSceneFromRigidbody(RigidBody *rb) const;
    PxSceneContainer* GetPxSceneContainerFromScene(Scene *scene);
    const PxSceneContainer* GetPxSceneContainerFromScene(Scene *scene) const;

    physx::PxFoundation* GetPxFoundation() const;
    physx::PxPhysics* GetPxPhysics() const;

    physx::PxRigidBody* CreatePxRigidBodyFromRigidBody(RigidBody *rb);

    static Vector2 GetVector2FromPxVec2(const physx::PxVec2 &v);
    static Vector3 GetVector3FromPxVec3(const physx::PxVec3 &v);
    static Vector4 GetVector4FromPxVec4(const physx::PxVec4 &v);
    static Quaternion GetQuaternionFromPxQuat(const physx::PxQuat &q);
    static physx::PxVec2 GetPxVec2FromVector2(const Vector2 &v);
    static physx::PxVec3 GetPxVec3FromVector3(const Vector3 &v);
    static physx::PxVec4 GetPxVec4FromVector4(const Vector4 &v);
    static physx::PxQuat GetPxQuatFromQuaternion(const Quaternion &q);

    static void FillTransformFromPxTransform(Transform *transform,
                                             const physx::PxTransform &pxTransform);
    static physx::PxTransform GetPxTransformFromTransform(Transform *tr);

    // IEventsComponentChangeGameObject
    virtual void OnComponentChangedGameObject(GameObject *previousGameObject,
                                              GameObject *newGameObject,
                                              Component *component) override;

    friend class PxSceneContainer;
};

class PxSceneContainer : public EventListener< IEventsObjectGatherer<RigidBody> >
{
public:
    PxSceneContainer(Scene *scene);
    virtual ~PxSceneContainer();

    Time::TimeT m_lastStepTimeMillis = 0;

    physx::PxScene *p_pxScene = nullptr;
    ObjectGatherer<RigidBody, true> *m_rigidBodyGatherer = nullptr;
    Map<RigidBody*, physx::PxRigidBody*> m_rigidBodyToPxRigidBody;
    Map<physx::PxRigidBody*, RigidBody*> m_pxRigidBodyToRigidBody;

    void ResetStepTimeReference();

    physx::PxScene *GetPxScene() const;
    physx::PxRigidBody* GetPxRigidBodyFromRigidBody(RigidBody *rb) const;
    RigidBody* GetRigidBodyFromPxRigidBody(physx::PxRigidBody *rb) const;

    // IEventsObjectGatherer
    virtual void OnObjectGathered(RigidBody *rb) override;
    virtual void OnObjectUnGathered(GameObject *previousGameObject,
                                    RigidBody *rb) override;
};

NAMESPACE_BANG_END

#endif // PHYSICS_H

