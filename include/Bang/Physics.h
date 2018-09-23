#ifndef PHYSICS_H
#define PHYSICS_H

#include "PxPhysicsAPI.h"

#include "Bang/Map.h"
#include "Bang/Time.h"
#include "Bang/Vector3.h"
#include "Bang/RayCastInfo.h"
#include "Bang/EventEmitter.h"
#include "Bang/EventListener.h"
#include "Bang/RayCastHitInfo.h"
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

    void Step(Scene *scene, Time simulationTime);
    void StepIfNeeded(Scene *scene);
    void ResetStepTimeReference(Scene *scene);
    void UpdateFromTransforms(Scene *scene);
    void SetIgnoreNextFrames(Scene *scene, int numNextFramesToIgnore);

    void RegisterScene(Scene *scene);
    void UnRegisterScene(Scene *scene);
    void RegisterPhysicsMaterial(PhysicsMaterial *physicsMaterial);

    void SetStepSleepTime(Time stepSleepTime);
    void SetMaxSubSteps(int maxSubSteps);
    void SetGravity(const Vector3 &gravity);

    int GetMaxSubSteps() const;
    Time GetStepSleepTime() const;
    const Vector3& GetGravity() const;
    PxSceneContainer* GetPxSceneContainerFromScene(Scene *scene);
    const PxSceneContainer* GetPxSceneContainerFromScene(Scene *scene) const;

    static void RayCast(const RayCastInfo &rcInfo, RayCastHitInfo *hitInfo);
    static void RayCast(const Vector3 &origin,
                        const Vector3 &direction,
                        float maxDistance,
                        RayCastHitInfo *hitInfo);

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
    Time m_stepSleepTime;
    Vector3 m_gravity = Vector3(0.0f, -30.0f, 0.0f);

    Map<Scene*, PxSceneContainer*> m_sceneToPxSceneContainer;

    Scene* GetSceneFromPhysicsObject(PhysicsObject *phObj) const;

    physx::PxFoundation* GetPxFoundation() const;
    physx::PxPhysics* GetPxPhysics() const;

    physx::PxMaterial* CreateNewMaterial();
    physx::PxRigidDynamic *CreateNewPxRigidDynamic(Transform *transform = nullptr);

    static GameObject* GetGameObjectFromPhysicsObject(PhysicsObject *phObj);

    static void FillTransformFromPxTransform(Transform *transform,
                                             const physx::PxTransform &pxTransform);
    static physx::PxTransform GetPxTransformFromTransform(Transform *tr);

    friend class RigidBody;
    friend class PxSceneContainer;
};


NAMESPACE_BANG_END

#endif // PHYSICS_H

