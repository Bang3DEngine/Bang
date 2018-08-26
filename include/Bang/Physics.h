#ifndef PHYSICS_H
#define PHYSICS_H

#include "PxPhysicsAPI.h"

#include "Bang/Map.h"
#include "Bang/Vector3.h"
#include "Bang/EventEmitter.h"
#include "Bang/EventListener.h"
#include "Bang/IEventsDestroy.h"

NAMESPACE_BANG_BEGIN

FORWARD class Scene;
FORWARD class RigidBody;
FORWARD class PhysicsObject;
FORWARD class PhysicsMaterial;
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
    void RegisterPhysicsMaterial(PhysicsMaterial *physicsMaterial);
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

    static GameObject* GetGameObjectFromPhysicsObject(PhysicsObject *phObj);

    static void FillTransformFromPxTransform(Transform *transform,
                                             const physx::PxTransform &pxTransform);
    static physx::PxTransform GetPxTransformFromTransform(Transform *tr);

    friend class PxSceneContainer;
};


NAMESPACE_BANG_END

#endif // PHYSICS_H

