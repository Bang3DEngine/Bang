#include "Bang/Physics.h"

#include "PxPhysicsAPI.h"

#include "Bang/Scene.h"
#include "Bang/RigidBody.h"
#include "Bang/Transform.h"
#include "Bang/Application.h"

USING_NAMESPACE_BANG

Physics::Physics()
{
}

Physics::~Physics()
{
    GetPxFoundation()->release();
    GetPxPhysics()->release();

    for (auto &it : m_sceneToPxSceneContainer)
    {
        PxSceneContainer *pxSceneCont = it.second;
        delete pxSceneCont;
    }
}

void Physics::Init()
{
    m_pxFoundation = PxCreateFoundation(PX_FOUNDATION_VERSION,
                                        m_pxAllocator,
                                        m_pxErrorCallback);

    m_pxPhysics = PxCreatePhysics(PX_PHYSICS_VERSION,
                                  *m_pxFoundation,
                                  physx::PxTolerancesScale(),
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
        physx::PxScene *pxScene = pxSceneContainer->GetPxScene();
        ASSERT(pxScene);

        const auto &allRBs = pxSceneContainer->m_rigidBodyGatherer->GetList();
        for (RigidBody *rb : allRBs)
        {
            if (GameObject *rbGo = rb->GetGameObject())
            {
                if (Transform *tr = rbGo->GetTransform())
                {
                    physx::PxRigidBody *pxRB = pxSceneContainer->
                                               GetPxRigidBodyFromRigidBody(rb);
                    pxRB->setGlobalPose( GetPxTransformFromTransform(tr) );
                }
            }
        }
    }
}

void Physics::Step(Scene *scene, float simulationTime)
{
    PxSceneContainer *pxSceneContainer = GetPxSceneContainerFromScene(scene);
    ASSERT(pxSceneContainer);

    physx::PxScene *pxScene = pxSceneContainer->GetPxScene();
    ASSERT(pxScene);

    // Step
    ResetStepTimeReference(scene);
    pxScene->simulate(simulationTime);
    pxScene->fetchResults(true);

    uint32_t numActActorsOut;
    physx::PxActor** activeActors = pxScene->getActiveActors(numActActorsOut);
    for (uint32_t i = 0; i < numActActorsOut; ++i)
    {
        physx::PxActor *activeActor = activeActors[i];
        physx::PxRigidBody *pxRB = SCAST<physx::PxRigidBody*>(activeActor);
        pxRB->getLinearVelocity();

        RigidBody *rb = pxSceneContainer->GetRigidBodyFromPxRigidBody(pxRB);

        GameObject *rbGo = rb->GetGameObject();
        ASSERT(rbGo);

        if (Transform *tr = rbGo->GetTransform())
        {
            FillTransformFromPxTransform(tr, pxRB->getGlobalPose() );
        }
    }
}

void Physics::StepIfNeeded(Scene *scene)
{
    PxSceneContainer *pxSceneContainer = GetPxSceneContainerFromScene(scene);
    ASSERT(pxSceneContainer);

    Time::TimeT nowMillis = Time::GetNow_Millis();
    float secondsSinceLastStep =
                (nowMillis - pxSceneContainer->m_lastStepTimeMillis) / 1000.0f;
    if (secondsSinceLastStep > GetSleepStepTimeSeconds())
    {
        Step(scene, secondsSinceLastStep);
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
    if (PxSceneContainer *pxSceneCont = GetPxSceneContainerFromScene(scene))
    {
        delete pxSceneCont;
        m_sceneToPxSceneContainer.Remove(scene);
    }
}

void Physics::RegisterRigidbody(RigidBody *rb)
{
    // rb->EventEmitter<IEventsComponentChangeGameObject>::RegisterListener(this);
    // if (rb->GetGameObject())
    // {
    //     CreatePxRigidBodyFromRigidBody(rb);
    // }
}

void Physics::UnRegisterRigidbody(RigidBody *rb)
{
    if (Scene *scene = GetSceneFromRigidbody(rb))
    {
        if (PxSceneContainer *pxSceneCont = GetPxSceneContainerFromScene(scene))
        {
            if (physx::PxRigidBody *pxRB = pxSceneCont->GetPxRigidBodyFromRigidBody(rb))
            {
                pxSceneCont->m_rigidBodyToPxRigidBody.Remove(rb);
                pxSceneCont->m_pxRigidBodyToRigidBody.Remove(pxRB);
            }
        }
    }

}

float Physics::GetSleepStepTimeSeconds() const
{
    return m_sleepStepTimeSeconds;
}

Physics *Physics::GetInstance()
{
    return Application::GetInstance()->GetPhysics();
}

Scene *Physics::GetSceneFromRigidbody(RigidBody *rb) const
{
    if (rb->GetGameObject())
    {
        return rb->GetGameObject()->GetScene();
    }
    return nullptr;
}

physx::PxFoundation *Physics::GetPxFoundation() const
{
    return m_pxFoundation;
}

physx::PxPhysics *Physics::GetPxPhysics() const
{
    return m_pxPhysics;
}

physx::PxRigidBody* Physics::CreatePxRigidBodyFromRigidBody(RigidBody *rb)
{
    ASSERT(!GetPxSceneContainerFromScene(GetSceneFromRigidbody(rb))->
           m_rigidBodyToPxRigidBody.ContainsKey(rb));

    GameObject *rbGo = rb->GetGameObject();
    ASSERT(rbGo);

    Scene *scene = GetSceneFromRigidbody(rb);
    ASSERT(scene);

    physx::PxRigidBody *pxRb = nullptr;
    if (PxSceneContainer *pxSceneCont = GetPxSceneContainerFromScene(scene))
    {
        physx::PxTransform pxTransform =  GetPxTransformFromTransform(
                                                        rbGo->GetTransform());
        physx::PxMaterial *pxMaterial = GetPxPhysics()->createMaterial(0.5f,
                                                                       0.5f,
                                                                       0.5f);
        pxRb = physx::PxCreateDynamic(*GetPxPhysics(),
                                      pxTransform,
                                      physx::PxSphereGeometry(3.0f),
                                      *pxMaterial,
                                      10.0f);

        pxSceneCont->m_rigidBodyToPxRigidBody.Add(rb, pxRb);
        pxSceneCont->m_pxRigidBodyToRigidBody.Add(pxRb, rb);

        physx::PxRigidDynamic *pxRd = SCAST<physx::PxRigidDynamic*>(pxRb);
        pxSceneCont->GetPxScene()->addActor(*pxRd);
    }
    return pxRb;
}

Vector2 Physics::GetVector2FromPxVec2(const physx::PxVec2 &v)
{
    return Vector2(v.x, v.y);
}

Vector3 Physics::GetVector3FromPxVec3(const physx::PxVec3 &v)
{
    return Vector3(v.x, v.y, v.z);
}

Vector4 Physics::GetVector4FromPxVec4(const physx::PxVec4 &v)
{
    return Vector4(v.x, v.y, v.z, v.w);
}

Quaternion Physics::GetQuaternionFromPxQuat(const physx::PxQuat &q)
{
    return Quaternion(q.x, q.y, q.z, q.w);
}

physx::PxVec2 Physics::GetPxVec2FromVector2(const Vector2 &v)
{
    return physx::PxVec2(v.x, v.y);
}

physx::PxVec3 Physics::GetPxVec3FromVector3(const Vector3 &v)
{
    return physx::PxVec3(v.x, v.y, v.z);
}

physx::PxVec4 Physics::GetPxVec4FromVector4(const Vector4 &v)
{
    return physx::PxVec4(v.x, v.y, v.z, v.w);
}

physx::PxQuat Physics::GetPxQuatFromQuaternion(const Quaternion &q)
{
    return physx::PxQuat(q.x, q.y, q.z, q.w);
}

void Physics::FillTransformFromPxTransform(Transform *transform,
                                           const physx::PxTransform &pxTransform)
{
    if (transform)
    {
        transform->SetPosition( GetVector3FromPxVec3(pxTransform.p) );
        transform->SetRotation( GetQuaternionFromPxQuat(pxTransform.q) );
    }
}

physx::PxTransform Physics::GetPxTransformFromTransform(Transform *tr)
{
    physx::PxTransform pxTransform;
    if (tr)
    {
        pxTransform.p = Physics::GetPxVec3FromVector3( tr->GetPosition() );
        pxTransform.q = Physics::GetPxQuatFromQuaternion( tr->GetRotation() );
    }
    return pxTransform;
}

void Physics::OnComponentChangedGameObject(GameObject *previousGameObject,
                                           GameObject *newGameObject,
                                           Component *component)
{
    /*
    if (RigidBody *rb = DCAST<RigidBody*>(component))
    {
        if (Scene *scene = GetSceneFromRigidbody(rb))
        {
            if (PxSceneContainer *pxSceneCont = GetPxSceneContainerFromScene(scene))
            {
                bool hasBeenCreatedBefore =
                    (pxSceneCont->GetPxRigidBodyFromRigidBody(rb) != nullptr);
                if (!hasBeenCreatedBefore)
                {
                    CreatePxRigidBodyFromRigidBody(rb);
                }
            }
        }
    }
    */
}

// PxSceneContainer
PxSceneContainer::PxSceneContainer(Scene *scene)
{
    Physics *ph = Physics::GetInstance();

    physx::PxSceneDesc sceneDesc(ph->GetPxPhysics()->getTolerancesScale());
    sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
    sceneDesc.cpuDispatcher	= physx::PxDefaultCpuDispatcherCreate(2);
    sceneDesc.filterShader	= physx::PxDefaultSimulationFilterShader;

    physx::PxScene *pxScene = ph->GetPxPhysics()->createScene(sceneDesc);
    pxScene->setFlag(physx::PxSceneFlag::eENABLE_ACTIVE_ACTORS, true);

    m_rigidBodyGatherer = new ObjectGatherer<RigidBody, true>();

    p_pxScene = pxScene;
    m_rigidBodyGatherer->SetRoot(scene);
    m_rigidBodyGatherer->EventEmitter<IEventsObjectGatherer>::
                         RegisterListener(this);
}

PxSceneContainer::~PxSceneContainer()
{
    p_pxScene->release();

    delete m_rigidBodyGatherer;
    for (auto &it : m_rigidBodyToPxRigidBody)
    {
        physx::PxRigidBody *pxRB = it.second;
        pxRB->release();
    }
}

void PxSceneContainer::ResetStepTimeReference()
{
    m_lastStepTimeMillis = Time::GetNow_Millis();
}

physx::PxScene *PxSceneContainer::GetPxScene() const
{
    return p_pxScene;
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

physx::PxRigidBody* PxSceneContainer::GetPxRigidBodyFromRigidBody(RigidBody *rb) const
{
    if (m_rigidBodyToPxRigidBody.ContainsKey(rb))
    {
        return m_rigidBodyToPxRigidBody.Get(rb);
    }
    return nullptr;
}

RigidBody *PxSceneContainer::GetRigidBodyFromPxRigidBody(physx::PxRigidBody *rb) const
{
    if (m_pxRigidBodyToRigidBody.ContainsKey(rb))
    {
        return m_pxRigidBodyToRigidBody.Get(rb);
    }
    return nullptr;
}

void PxSceneContainer::OnObjectGathered(RigidBody *rb)
{
    Physics::GetInstance()->CreatePxRigidBodyFromRigidBody(rb);
}

void PxSceneContainer::OnObjectUnGathered(GameObject *previousGameObject,
                                          RigidBody *rb)
{
    (void) previousGameObject;
    Physics::GetInstance()->UnRegisterRigidbody(rb);
}
