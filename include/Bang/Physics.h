#ifndef PHYSICS_H
#define PHYSICS_H

#include <functional>
#include <vector>

#include "Bang/Array.tcc"
#include "Bang/BangDefines.h"
#include "Bang/EventEmitter.tcc"
#include "Bang/EventListener.h"
#include "Bang/IEventsDestroy.h"
#include "Bang/Map.h"
#include "Bang/Time.h"
#include "Bang/Vector3.h"
#include "extensions/PxDefaultAllocator.h"
#include "extensions/PxDefaultErrorCallback.h"
#include "foundation/PxQuat.h"
#include "foundation/PxTransform.h"
#include "foundation/PxVec2.h"
#include "foundation/PxVec3.h"
#include "foundation/PxVec4.h"

namespace physx
{
class PxCooking;
class PxFoundation;
class PxMaterial;
class PxGeometry;
class PxPhysics;
class PxRigidBody;
class PxRigidStatic;
class PxRigidDynamic;
class PxTriangleMesh;
}

namespace Bang
{
template <class>
class EventEmitter;
class GameObject;
class Collider;
class IEventsDestroy;
class Mesh;
class PhysicsMaterial;
class PhysicsComponent;
class PxSceneContainer;
class Scene;
class Transform;
struct RayCastHitInfo;
struct RayCastInfo;

class Physics
{
public:
    Physics();
    virtual ~Physics();

    void Init();

    void Step(Scene *scene, Time simulationTime);
    void StepIfNeeded(Scene *scene);
    void ResetStepTimeReference(Scene *scene);
    void UpdatePxSceneFromTransforms(Scene *scene);
    void SetIgnoreNextFrames(Scene *scene, int numNextFramesToIgnore);

    void RegisterScene(Scene *scene);
    void UnRegisterScene(Scene *scene);
    void RegisterPhysicsMaterial(PhysicsMaterial *physicsMaterial);

    void SetStepSleepTime(Time stepSleepTime);
    void SetMaxSubSteps(int maxSubSteps);
    void SetGravity(const Vector3 &gravity);

    int GetMaxSubSteps() const;
    Time GetStepSleepTime() const;
    const Vector3 &GetGravity() const;

    PxSceneContainer *GetPxSceneContainerFromScene(Scene *scene);
    const PxSceneContainer *GetPxSceneContainerFromScene(Scene *scene) const;

    static void RayCast(const RayCastInfo &rcInfo, RayCastHitInfo *hitInfo);
    static void RayCast(const Vector3 &origin,
                        const Vector3 &direction,
                        float maxDistance,
                        RayCastHitInfo *hitInfo);
    static bool Overlap(const physx::PxGeometry &pxGeometry0,
                        const physx::PxTransform &pxTransform0,
                        const physx::PxGeometry &pxGeometry1,
                        const physx::PxTransform &pxTransform1);
    static bool Overlap(const Collider *collider0,
                        const physx::PxGeometry &pxGeometry1,
                        const physx::PxTransform &pxTransform1);
    static bool Overlap(const Collider *collider0, const Collider *collider1);

    physx::PxRigidActor *CreateNewPxRigidActor(bool isStatic = false,
                                               Transform *transform = nullptr);
    physx::PxTriangleMesh *CreatePxTriangleMesh(Mesh *mesh) const;
    static physx::PxMaterial *GetDefaultPxMaterial();

    static Vector2 GetVector2FromPxVec2(const physx::PxVec2 &v);
    static Vector3 GetVector3FromPxVec3(const physx::PxVec3 &v);
    static Vector4 GetVector4FromPxVec4(const physx::PxVec4 &v);
    static Quaternion GetQuaternionFromPxQuat(const physx::PxQuat &q);
    static physx::PxVec2 GetPxVec2FromVector2(const Vector2 &v);
    static physx::PxVec3 GetPxVec3FromVector3(const Vector3 &v);
    static physx::PxVec4 GetPxVec4FromVector4(const Vector4 &v);
    static physx::PxQuat GetPxQuatFromQuaternion(const Quaternion &q);
    static physx::PxTransform GetPxTransformFromMatrix(const Matrix4 &m);
    static physx::PxTransform GetPxTransformFromTransform(Transform *tr);
    static Physics *GetInstance();

private:
    physx::PxDefaultAllocator m_pxAllocator;
    physx::PxDefaultErrorCallback m_pxErrorCallback;
    physx::PxFoundation *m_pxFoundation = nullptr;
    physx::PxPhysics *m_pxPhysics = nullptr;
    physx::PxCooking *m_pxCooking = nullptr;

    int m_maxSubSteps = 3;
    Time m_stepSleepTime;
    Vector3 m_gravity = Vector3(0.0f, -30.0f, 0.0f);

    Map<Scene *, PxSceneContainer *> m_sceneToPxSceneContainer;

    Scene *GetSceneFromPhysicsComponent(PhysicsComponent *phComp) const;

    physx::PxFoundation *GetPxFoundation() const;
    physx::PxPhysics *GetPxPhysics() const;

    physx::PxMaterial *CreateNewMaterial();

    static void FillTransformFromPxTransform(
        Transform *transform,
        const physx::PxTransform &pxTransform);

    friend class RigidBody;
    friend class PxSceneContainer;
};
}

#endif  // PHYSICS_H
