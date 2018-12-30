#include "Bang/Physics.h"

#include <stdint.h>
#include <utility>

#include "Bang/Application.h"
#include "Bang/Array.h"
#include "Bang/Assert.h"
#include "Bang/AssetHandle.h"
#include "Bang/ClassDB.h"
#include "Bang/Collider.h"
#include "Bang/Component.h"
#include "Bang/Debug.h"
#include "Bang/EventEmitter.h"
#include "Bang/EventListener.tcc"
#include "Bang/GameObject.h"
#include "Bang/GameObject.tcc"
#include "Bang/IEventsDestroy.h"
#include "Bang/Map.tcc"
#include "Bang/MaterialFactory.h"
#include "Bang/Math.h"
#include "Bang/Mesh.h"
#include "Bang/ObjectGatherer.h"
#include "Bang/ObjectGatherer.tcc"
#include "Bang/PhysicsComponent.h"
#include "Bang/PhysicsMaterial.h"
#include "Bang/PxSceneContainer.h"
#include "Bang/RayCastInfo.h"
#include "Bang/RigidBody.h"
#include "Bang/Scene.h"
#include "Bang/SceneManager.h"
#include "Bang/Transform.h"
#include "PxGeometryQuery.h"
#include "PxPhysics.h"
#include "PxPhysicsVersion.h"
#include "PxRigidActor.h"
#include "PxRigidBody.h"
#include "PxRigidDynamic.h"
#include "PxRigidStatic.h"
#include "PxScene.h"
#include "common/PxCoreUtilityTypes.h"
#include "common/PxTolerancesScale.h"
#include "cooking/PxBVH33MidphaseDesc.h"
#include "cooking/PxCooking.h"
#include "cooking/PxTriangleMeshDesc.h"
#include "extensions/PxSimpleFactory.h"
#include "foundation/Px.h"
#include "foundation/PxFoundation.h"
#include "foundation/PxFoundationVersion.h"
#include "geometry/PxSphereGeometry.h"

namespace Bang
{
struct RayCastHitInfo;
}

namespace physx
{
class PxActor;
class PxMaterial;
class PxShape;
class PxTriangleMesh;
}  // namespace physx

using namespace physx;
using namespace Bang;

Physics::Physics()
{
    m_stepSleepTime.SetSeconds(1.0 / 30.0);
}

Physics::~Physics()
{
    for (auto &it : m_sceneToPxSceneContainer)
    {
        PxSceneContainer *pxSceneCont = it.second;
        delete pxSceneCont;
    }

    GetPxPhysics()->release();
    GetPxFoundation()->release();
}

void Physics::Init()
{
    m_pxFoundation = PxCreateFoundation(
        PX_FOUNDATION_VERSION, m_pxAllocator, m_pxErrorCallback);
    if (!m_pxFoundation)
    {
        Debug_Error("PxFoundation creation failed!");
        Application::Exit(1, true);
    }

    m_pxPhysics = PxCreatePhysics(PX_PHYSICS_VERSION,
                                  *m_pxFoundation,
                                  PxTolerancesScale(),
                                  true,
                                  nullptr);
    if (!m_pxPhysics)
    {
        Debug_Error("PxPhysics creation failed!");
        Application::Exit(1, true);
    }

    m_pxCooking = PxCreateCooking(PX_PHYSICS_VERSION,
                                  *m_pxFoundation,
                                  PxCookingParams(PxTolerancesScale()));
    if (!m_pxCooking)
    {
        Debug_Error("PxCooking creation failed!");
        Application::Exit(1, true);
    }
}

void Physics::ResetStepTimeReference(Scene *scene)
{
    if (PxSceneContainer *pxSceneCont = GetPxSceneContainerFromScene(scene))
    {
        pxSceneCont->ResetStepTimeReference();
    }
}

void Physics::UpdatePxSceneFromTransforms(Scene *scene)
{
    if (PxSceneContainer *pxSceneContainer =
            GetPxSceneContainerFromScene(scene))
    {
        const auto &allPhObjs =
            pxSceneContainer->m_physicsObjectGatherer->GetGatheredObjects();
        for (PhysicsComponent *phComp : allPhObjs)
        {
            if (GameObject *phCompGo = phComp->GetGameObject())
            {
                if (PxActor *pxActor =
                        pxSceneContainer->GetPxActorFromGameObject(phCompGo))
                {
                    if (Transform *tr = phCompGo->GetTransform())
                    {
                        PxRigidActor *pxRA = SCAST<PxRigidActor *>(pxActor);
                        pxRA->setGlobalPose(GetPxTransformFromTransform(tr));
                    }
                }
            }
        }
    }
}

void Physics::SetIgnoreNextFrames(Scene *scene, int numNextFramesToIgnore)
{
    if (PxSceneContainer *pxSceneContainer =
            GetPxSceneContainerFromScene(scene))
    {
        pxSceneContainer->m_numFramesLeftToIgnore = numNextFramesToIgnore;
    }
}

void Physics::Step(Scene *scene, Time simulationTime)
{
    PxSceneContainer *pxSceneContainer = GetPxSceneContainerFromScene(scene);
    ASSERT(pxSceneContainer);

    PxScene *pxScene = pxSceneContainer->GetPxScene();
    ASSERT(pxScene);

    // Step
    constexpr double MaxSimulationTimeSeconds = 0.1;
    simulationTime.SetSeconds(
        Math::Min(simulationTime.GetSeconds(), MaxSimulationTimeSeconds));
    int subStepsToBeDone =
        Math::Min(SCAST<int>(Math::Ceil(simulationTime.GetSeconds() /
                                        GetStepSleepTime().GetSeconds())),
                  GetMaxSubSteps());
    subStepsToBeDone = Math::Max(subStepsToBeDone, 1);

    Time subStepTime = (simulationTime / subStepsToBeDone);
    for (int i = 0; i < subStepsToBeDone; ++i)
    {
        pxScene->simulate(subStepTime.GetSeconds());
        pxScene->fetchResults(true);
    }
    ResetStepTimeReference(scene);

    uint32_t numActActorsOut;
    PxActor **activeActors = pxScene->getActiveActors(numActActorsOut);
    for (uint32_t i = 0; i < numActActorsOut; ++i)
    {
        PxActor *pxActor = activeActors[i];
        GameObject *go = pxSceneContainer->GetGameObjectFromPxActor(pxActor);
        if (!go)
        {
            continue;
        }

        Array<PhysicsComponent *> physicsObjects =
            go->GetComponents<PhysicsComponent>();
        for (PhysicsComponent *phComp : physicsObjects)
        {
            switch (phComp->GetPhysicsComponentType())
            {
                case PhysicsComponent::Type::RIGIDBODY:
                {
                    RigidBody *rb = SCAST<RigidBody *>(phComp);
                    if (rb->IsActiveRecursively())
                    {
                        PxRigidActor *pxRA = SCAST<PxRigidActor *>(pxActor);
                        if (Transform *tr = go->GetTransform())
                        {
                            FillTransformFromPxTransform(tr,
                                                         pxRA->getGlobalPose());
                        }
                    }
                }
                break;

                default: break;
            }
        }
    }
}

void Physics::StepIfNeeded(Scene *scene)
{
    if (PxSceneContainer *pxSceneContainer =
            GetPxSceneContainerFromScene(scene))
    {
        Time now = Time::GetNow();
        Time timeSinceLastStep = (now - pxSceneContainer->m_lastStepTime);
        if (timeSinceLastStep >= GetStepSleepTime())
        {
            if (pxSceneContainer->m_numFramesLeftToIgnore <= 0)
            {
                Step(scene, timeSinceLastStep);
            }
            else
            {
                pxSceneContainer->ResetStepTimeReference();
                --pxSceneContainer->m_numFramesLeftToIgnore;
            }
        }
    }
}

void Physics::SetStepSleepTime(Time stepSleepTime)
{
    if (stepSleepTime != GetStepSleepTime())
    {
        m_stepSleepTime = stepSleepTime;
    }
}

void Physics::SetMaxSubSteps(int maxSubSteps)
{
    if (maxSubSteps != GetMaxSubSteps())
    {
        m_maxSubSteps = maxSubSteps;
    }
}

void Physics::SetGravity(const Vector3 &gravity)
{
    if (gravity != GetGravity())
    {
        m_gravity = gravity;
        for (const auto &pair : m_sceneToPxSceneContainer)
        {
            PxSceneContainer *pxSceneCont = pair.second;
            if (PxScene *pxScene = pxSceneCont->GetPxScene())
            {
                pxScene->setGravity(
                    Physics::GetPxVec3FromVector3(GetGravity()));
            }
        }
    }
}

void Physics::RegisterScene(Scene *scene)
{
    ASSERT(scene);

    PxSceneContainer *pxSceneContainer = new PxSceneContainer(scene);
    m_sceneToPxSceneContainer.Add(scene, pxSceneContainer);
}

void Physics::UnRegisterScene(Scene *scene)
{
    if (m_sceneToPxSceneContainer.ContainsKey(scene))
    {
        PxSceneContainer *pxSceneCont = m_sceneToPxSceneContainer.Get(scene);
        delete pxSceneCont;

        m_sceneToPxSceneContainer.Remove(scene);
    }
}

void Physics::RegisterPhysicsMaterial(PhysicsMaterial *physicsMaterial)
{
    PxMaterial *pxMaterial = GetPxPhysics()->createMaterial(0.5f, 0.5f, 0.5f);
    physicsMaterial->SetPxMaterial(pxMaterial);
    physicsMaterial->UpdatePxMaterial();
}

int Physics::GetMaxSubSteps() const
{
    return m_maxSubSteps;
}

Time Physics::GetStepSleepTime() const
{
    return m_stepSleepTime;
}

const Vector3 &Physics::GetGravity() const
{
    return m_gravity;
}

PxTriangleMesh *Physics::CreatePxTriangleMesh(Mesh *mesh) const
{
    PxTriangleMesh *pxTriangleMesh = nullptr;
    if (mesh)
    {
        PxTolerancesScale scale;
        PxCookingParams params(scale);
        params.meshPreprocessParams.clear(
            PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH);
        params.meshPreprocessParams.clear(
            PxMeshPreprocessingFlag::eWELD_VERTICES);
        params.meshPreprocessParams.set(
            PxMeshPreprocessingFlag::eDISABLE_ACTIVE_EDGES_PRECOMPUTE);
        params.meshCookingHint = PxMeshCookingHint::eCOOKING_PERFORMANCE;

        m_pxCooking->setParams(params);

        PxTriangleMeshDesc meshDesc;
        meshDesc.points.count = mesh->GetPositionsPool().Size();
        meshDesc.points.stride = sizeof(Vector3);
        meshDesc.points.data = mesh->GetPositionsPool().Data();

        meshDesc.triangles.count = mesh->GetNumTriangles();
        meshDesc.triangles.stride = 3 * sizeof(uint);
        meshDesc.triangles.data = mesh->GetTrianglesVertexIds().Data();

#ifdef DEBUG
        if (!meshDesc.isValid())
        {
            Debug_Warn("Mesh description is not valid.");
        }

        if (!m_pxCooking->validateTriangleMesh(meshDesc))
        {
            Debug_Warn("Triangle mesh " << mesh
                                        << " not optimal for collider.");
        }
#endif

        pxTriangleMesh = m_pxCooking->createTriangleMesh(
            meshDesc, GetPxPhysics()->getPhysicsInsertionCallback());
    }

    return pxTriangleMesh;
}

PxMaterial *Physics::GetDefaultPxMaterial()
{
    PhysicsMaterial *phDefaultMat =
        MaterialFactory::GetDefaultPhysicsMaterial().Get();
    PxMaterial *pxDefaultMat = phDefaultMat->GetPxMaterial();
    return pxDefaultMat;
}

void Physics::RayCast(const RayCastInfo &rcInfo, RayCastHitInfo *hitInfo)
{
    Physics *ph = Physics::GetInstance();
    ASSERT(ph);

    if (Scene *scene = SceneManager::GetActiveScene())
    {
        if (PxSceneContainer *pxSceneCont =
                ph->GetPxSceneContainerFromScene(scene))
        {
            pxSceneCont->RayCast(rcInfo, hitInfo);
        }
    }
}

void Physics::RayCast(const Vector3 &origin,
                      const Vector3 &direction,
                      float maxDistance,
                      RayCastHitInfo *hitInfo)
{
    RayCastInfo rcInfo;
    rcInfo.origin = origin;
    rcInfo.direction = direction;
    rcInfo.maxDistance = maxDistance;
    Physics::RayCast(rcInfo, hitInfo);
}

bool Physics::Overlap(const PxGeometry &pxGeometry0,
                      const PxTransform &pxTransform0,
                      const PxGeometry &pxGeometry1,
                      const PxTransform &pxTransform1)
{
    return PxGeometryQuery::overlap(
        pxGeometry0, pxTransform0, pxGeometry1, pxTransform1);
}

bool Physics::Overlap(const Collider *collider0,
                      const PxGeometry &pxGeometry1,
                      const PxTransform &pxTransform1)
{
    if (!collider0 || !collider0->GetPxRigidActor())
    {
        return false;
    }
    return Physics::Overlap(collider0->GetPxShape()->getGeometry().any(),
                            collider0->GetWorldPxTransform(),
                            pxGeometry1,
                            pxTransform1);
}

bool Physics::Overlap(const Collider *collider0, const Collider *collider1)
{
    if (!collider1 || !collider1->GetPxRigidActor())
    {
        return false;
    }
    return Physics::Overlap(collider0,
                            collider1->GetPxShape()->getGeometry().any(),
                            collider1->GetWorldPxTransform());
}

Physics *Physics::GetInstance()
{
    return Application::GetInstance()->GetPhysics();
}

Scene *Physics::GetSceneFromPhysicsComponent(PhysicsComponent *phComp) const
{
    if (GameObject *phCompGo = phComp->GetGameObject())
    {
        return phCompGo->GetScene();
    }
    return nullptr;
}

PxFoundation *Physics::GetPxFoundation() const
{
    return m_pxFoundation;
}

PxPhysics *Physics::GetPxPhysics() const
{
    return m_pxPhysics;
}

PxMaterial *Physics::CreateNewMaterial()
{
    return GetPxPhysics()->createMaterial(0.5f, 0.5f, 0.02f);
}

PxRigidActor *Physics::CreateNewPxRigidActor(bool isStatic,
                                             Transform *transform)
{
    PxMaterial *pxDefaultMat =
        MaterialFactory::GetDefaultPhysicsMaterial().Get()->GetPxMaterial();

    PxTransform pxTransform = GetPxTransformFromTransform(transform);
    PxRigidActor *pxRA = nullptr;
    if (isStatic)
    {
        PxRigidStatic *pxRS = PxCreateStatic(*GetPxPhysics(),
                                             pxTransform,
                                             PxSphereGeometry(0.01f),
                                             *pxDefaultMat);
        pxRA = pxRS;
    }
    else
    {
        PxRigidDynamic *pxRD = PxCreateDynamic(*GetPxPhysics(),
                                               pxTransform,
                                               PxSphereGeometry(0.01f),
                                               *pxDefaultMat,
                                               10.0f);
        pxRD->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
        pxRA = pxRD;
    }

    // Remove default initial shape
    PxShape *shape;
    pxRA->getShapes(&shape, 1);
    pxRA->detachShape(*shape);

    return pxRA;
}

Vector2 Physics::GetVector2FromPxVec2(const PxVec2 &v)
{
    return Vector2(v.x, v.y);
}

Vector3 Physics::GetVector3FromPxVec3(const PxVec3 &v)
{
    return Vector3(v.x, v.y, v.z);
}

Vector4 Physics::GetVector4FromPxVec4(const PxVec4 &v)
{
    return Vector4(v.x, v.y, v.z, v.w);
}

Quaternion Physics::GetQuaternionFromPxQuat(const PxQuat &q)
{
    return Quaternion(q.x, q.y, q.z, q.w);
}

PxVec2 Physics::GetPxVec2FromVector2(const Vector2 &v)
{
    return PxVec2(v.x, v.y);
}

PxVec3 Physics::GetPxVec3FromVector3(const Vector3 &v)
{
    return PxVec3(v.x, v.y, v.z);
}

PxVec4 Physics::GetPxVec4FromVector4(const Vector4 &v)
{
    return PxVec4(v.x, v.y, v.z, v.w);
}

PxQuat Physics::GetPxQuatFromQuaternion(const Quaternion &q)
{
    return PxQuat(q.x, q.y, q.z, q.w);
}

PxTransform Physics::GetPxTransformFromMatrix(const Matrix4 &m)
{
    PxTransform pxTransform(PxIdentity);
    pxTransform.p = Physics::GetPxVec3FromVector3(m.GetTranslation());
    pxTransform.q = Physics::GetPxQuatFromQuaternion(m.GetRotation());
    if (!pxTransform.isValid())
    {
        pxTransform = PxTransform(PxIdentity);
    }
    return pxTransform;
}

void Physics::FillTransformFromPxTransform(Transform *transform,
                                           const PxTransform &pxTransform)
{
    if (transform)
    {
        transform->SetPosition(GetVector3FromPxVec3(pxTransform.p));
        transform->SetRotation(GetQuaternionFromPxQuat(pxTransform.q));
    }
}

PxTransform Physics::GetPxTransformFromTransform(Transform *tr)
{
    return GetPxTransformFromMatrix(tr ? tr->GetLocalToWorldMatrix()
                                       : Matrix4::Identity());
}

PxSceneContainer *Physics::GetPxSceneContainerFromScene(Scene *scene)
{
    if (m_sceneToPxSceneContainer.ContainsKey(scene))
    {
        return m_sceneToPxSceneContainer.Get(scene);
    }
    return nullptr;
}
const PxSceneContainer *Physics::GetPxSceneContainerFromScene(
    Scene *scene) const
{
    return const_cast<Physics *>(this)->GetPxSceneContainerFromScene(scene);
}
