#include "Bang/Physics.h"

#include "PxPhysicsAPI.h"

#include "Bang/Scene.h"
#include "Bang/RigidBody.h"
#include "Bang/Transform.h"
#include "Bang/Application.h"
#include "Bang/BoxCollider.h"
#include "Bang/SceneManager.h"
#include "Bang/PhysicsObject.h"
#include "Bang/SphereCollider.h"
#include "Bang/CapsuleCollider.h"
#include "Bang/MaterialFactory.h"
#include "Bang/PhysicsMaterial.h"
#include "Bang/PxSceneContainer.h"

using namespace physx;
USING_NAMESPACE_BANG

Physics::Physics()
{
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
    m_pxFoundation = PxCreateFoundation(PX_FOUNDATION_VERSION,
                                        m_pxAllocator,
                                        m_pxErrorCallback);

    m_pxPhysics = PxCreatePhysics(PX_PHYSICS_VERSION,
                                  *m_pxFoundation,
                                  PxTolerancesScale(),
                                  true,
                                  nullptr);
}

void Physics::ResetStepTimeReference(Scene *scene)
{
    if (PxSceneContainer *pxSceneCont = GetPxSceneContainerFromScene(scene))
    {
        pxSceneCont->ResetStepTimeReference();
    }
}

void Physics::UpdateFromTransforms(Scene *scene)
{
    if (PxSceneContainer *pxSceneContainer = GetPxSceneContainerFromScene(scene))
    {
        const auto &allPhObjs = pxSceneContainer->m_physicsObjectGatherer->
                                GetGatheredObjects();
        for (PhysicsObject *phObj : allPhObjs)
        {
            if (Component *comp = DCAST<Component*>(phObj))
            {
                if (GameObject *phObjGo = comp->GetGameObject())
                {
                    if (PxActor *pxActor =
                        pxSceneContainer->GetPxActorFromGameObject(phObjGo))
                    {
                        if (Transform *tr = phObjGo->GetTransform())
                        {
                            PxRigidBody *pxRB = SCAST<PxRigidBody*>(pxActor);
                            pxRB->setGlobalPose( GetPxTransformFromTransform(tr) );
                        }
                    }
                }
            }
        }
    }
}

void Physics::SetIgnoreNextFrames(Scene *scene, int numNextFramesToIgnore)
{
    if (PxSceneContainer *pxSceneContainer = GetPxSceneContainerFromScene(scene))
    {
        pxSceneContainer->m_numFramesLeftToIgnore = numNextFramesToIgnore;
    }
}

void Physics::Step(Scene *scene, float simulationTime)
{
    PxSceneContainer *pxSceneContainer = GetPxSceneContainerFromScene(scene);
    ASSERT(pxSceneContainer);

    PxScene *pxScene = pxSceneContainer->GetPxScene();
    ASSERT(pxScene);

    // Step
    constexpr float MaxSimulationTime = 0.1f;
    simulationTime = Math::Min(simulationTime, MaxSimulationTime);
    int subStepsToBeDone =
        Math::Min(
           SCAST<int>(Math::Ceil(simulationTime / GetStepSleepTimeSeconds())),
           GetMaxSubSteps());
    subStepsToBeDone = Math::Max(subStepsToBeDone, 1);

    float subStepTime = (simulationTime / subStepsToBeDone);
    for (int i = 0; i < subStepsToBeDone; ++i)
    {
        pxScene->simulate(subStepTime);
        pxScene->fetchResults(true);
    }
    ResetStepTimeReference(scene);

    uint32_t numActActorsOut;
    PxActor** activeActors = pxScene->getActiveActors(numActActorsOut);
    for (uint32_t i = 0; i < numActActorsOut; ++i)
    {
        PxActor *pxActor = activeActors[i];
        GameObject *go = pxSceneContainer->GetGameObjectFromPxActor(pxActor);
        if (!go)
        {
            continue;
        }

        Array<PhysicsObject*> physicsObjects = go->GetComponents<PhysicsObject>();
        for (PhysicsObject *phObj : physicsObjects)
        {
            switch (phObj->GetPhysicsObjectType())
            {
                case PhysicsObject::Type::RIGIDBODY:
                {
                    RigidBody *rb = SCAST<RigidBody*>(phObj);
                    if (rb->IsActive())
                    {
                        PxRigidBody *pxRB = SCAST<PxRigidBody*>(pxActor);
                        if (Transform *tr = go->GetTransform())
                        {
                            FillTransformFromPxTransform( tr, pxRB->getGlobalPose() );
                        }
                    }
                }
                break;

                default:
                break;
            }
        }
    }
}

void Physics::StepIfNeeded(Scene *scene)
{
    if (PxSceneContainer *pxSceneContainer = GetPxSceneContainerFromScene(scene))
    {
        Time::TimeT nowMillis = Time::GetNow_Millis();
        float secondsSinceLastStep =
                    (nowMillis - pxSceneContainer->m_lastStepTimeMillis) / 1000.0f;
        if (secondsSinceLastStep >= GetStepSleepTimeSeconds())
        {
            if (pxSceneContainer->m_numFramesLeftToIgnore <= 0)
            {
                Step(scene, secondsSinceLastStep);
            }
            else
            {
                pxSceneContainer->ResetStepTimeReference();
                --pxSceneContainer->m_numFramesLeftToIgnore;
            }
        }
    }
}

void Physics::SetStepSleepTime(float stepSleepTimeSeconds)
{
    if (stepSleepTimeSeconds != GetStepSleepTimeSeconds())
    {
        m_stepSleepTimeSeconds = stepSleepTimeSeconds;
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
            PxScene *pxScene = pxSceneCont->GetPxScene();
            pxScene->setGravity( Physics::GetPxVec3FromVector3(GetGravity()) );
        }
    }
}

void Physics::RegisterScene(Scene *scene)
{
    ASSERT(scene);

    PxSceneContainer *pxSceneContainer = new PxSceneContainer(scene);
    m_sceneToPxSceneContainer.Add(scene, pxSceneContainer);
    scene->EventEmitter<IEventsDestroy>::RegisterListener(this);
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

float Physics::GetStepSleepTimeSeconds() const
{
    return m_stepSleepTimeSeconds;
}

const Vector3 &Physics::GetGravity() const
{
    return m_gravity;
}

void Physics::RayCast(const RayCastInfo &rcInfo, RayCastHitInfo *hitInfo)
{
    Physics *ph = Physics::GetInstance();
    ASSERT(ph);

    if (Scene *scene = SceneManager::GetActiveScene())
    {
        if (PxSceneContainer *pxSceneCont = ph->GetPxSceneContainerFromScene(scene))
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

Physics *Physics::GetInstance()
{
    return Application::GetInstance()->GetPhysics();
}

void Physics::OnDestroyed(EventEmitter<IEventsDestroy> *ee)
{
    if (Scene *scene = DCAST<Scene*>(ee))
    {
        if (PxSceneContainer *pxSceneCont = GetPxSceneContainerFromScene(scene))
        {
            delete pxSceneCont;
            m_sceneToPxSceneContainer.Remove(scene);
        }
    }
}

Scene *Physics::GetSceneFromPhysicsObject(PhysicsObject *phObj) const
{
    if (GameObject *phObjGo = Physics::GetGameObjectFromPhysicsObject(phObj))
    {
        return phObjGo->GetScene();
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

PxRigidDynamic *Physics::CreateNewPxRigidDynamic(Transform *transform)
{
    PxMaterial *pxDefaultMat =
            MaterialFactory::GetDefaultPhysicsMaterial().Get()->GetPxMaterial();

    PxTransform pxTransform = GetPxTransformFromTransform(transform);
    PxRigidDynamic *pxRD =  PxCreateDynamic(*GetPxPhysics(),
                                            pxTransform,
                                            PxSphereGeometry(0.01f),
                                            *pxDefaultMat,
                                            10.0f);
    pxRD->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);

    // Remove default initial shape
    PxShape *shape;
    pxRD->getShapes(&shape, 1);
    pxRD->detachShape(*shape);

    return pxRD;
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

void Physics::FillTransformFromPxTransform(Transform *transform,
                                           const PxTransform &pxTransform)
{
    if (transform)
    {
        transform->SetPosition( GetVector3FromPxVec3(pxTransform.p) );
        transform->SetRotation( GetQuaternionFromPxQuat(pxTransform.q) );
    }
}

PxTransform Physics::GetPxTransformFromTransform(Transform *tr)
{
    PxTransform pxTransform(PxIdentity);
    if (tr)
    {
        pxTransform.p = Physics::GetPxVec3FromVector3( tr->GetPosition() );
        pxTransform.q = Physics::GetPxQuatFromQuaternion( tr->GetRotation() );
        if (!pxTransform.isValid())
        {
            pxTransform = PxTransform(PxIdentity);
        }
    }
    return pxTransform;
}

PxSceneContainer *Physics::GetPxSceneContainerFromScene(Scene *scene)
{
    if (m_sceneToPxSceneContainer.ContainsKey(scene))
    {
        return m_sceneToPxSceneContainer.Get(scene);
    }
    return nullptr;
}
const PxSceneContainer *Physics::GetPxSceneContainerFromScene(Scene *scene) const
{
    return const_cast<Physics*>(this)->GetPxSceneContainerFromScene(scene);
}

GameObject *Physics::GetGameObjectFromPhysicsObject(PhysicsObject *phObj)
{
    return DCAST<Component*>(phObj)->GetGameObject();
}
