#include "Bang/Physics.h"

#include "PxPhysicsAPI.h"

#include "Bang/Scene.h"
#include "Bang/RigidBody.h"
#include "Bang/Transform.h"
#include "Bang/Application.h"
#include "Bang/PhysicsObject.h"

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

        const auto &allPhObjs = pxSceneContainer->m_physicsObjectGatherer->GetList();
        for (PhysicsObject *phObj : allPhObjs)
        {
            if (Component *comp = DCAST<Component*>(phObj))
            {
                if (GameObject *phObjGo = comp->GetGameObject())
                {
                    if (physx::PxActor *pxActor =
                        pxSceneContainer->GetPxActorFromGameObject(phObjGo))
                    {
                        if (Transform *tr = phObjGo->GetTransform())
                        {
                            // RigidBody *rb = DCAST<RigidBody*>(phObj);
                            physx::PxRigidBody *pxRB =
                                         SCAST<physx::PxRigidBody*>(pxActor);
                            pxRB->setGlobalPose( GetPxTransformFromTransform(tr) );
                        }
                    }
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
        physx::PxActor *pxActor = activeActors[i];
        GameObject *go = pxSceneContainer->GetGameObjectFromPxActor(pxActor);

        List<PhysicsObject*> physicsObjects = go->GetComponents<PhysicsObject>();
        for (PhysicsObject *phObj : physicsObjects)
        {
            switch (phObj->GetPhysicsObjectType())
            {
                case PhysicsObject::Type::RIGIDBODY:
                {
                    RigidBody *rb = SCAST<RigidBody*>(phObj);
                    physx::PxRigidBody *pxRB = SCAST<physx::PxRigidBody*>(pxActor);
                    if (Transform *tr = go->GetTransform())
                    {
                        FillTransformFromPxTransform( tr, pxRB->getGlobalPose() );
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

float Physics::GetSleepStepTimeSeconds() const
{
    return m_sleepStepTimeSeconds;
}

Physics *Physics::GetInstance()
{
    return Application::GetInstance()->GetPhysics();
}

Scene *Physics::GetSceneFromPhysicsObject(PhysicsObject *phObj) const
{
    if (GameObject *phObjGo = Physics::GetGameObjectFromPhysicsObject(phObj))
    {
        return phObjGo->GetScene();
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

physx::PxActor* Physics::CreateIntoPxScene(PhysicsObject *phObj)
{
    GameObject *phObjGo = Physics::GetGameObjectFromPhysicsObject(phObj);
    ASSERT(phObjGo);

    ASSERT(!GetPxSceneContainerFromScene(GetSceneFromPhysicsObject(phObj))->
               m_gameObjectToPxActor.ContainsKey(phObjGo));

    Scene *scene = GetSceneFromPhysicsObject(phObj);
    ASSERT(scene);

    physx::PxActor *pxActor = nullptr;
    if (PxSceneContainer *pxSceneCont = GetPxSceneContainerFromScene(scene))
    {
        physx::PxTransform pxTransform =  GetPxTransformFromTransform(
                                                        phObjGo->GetTransform());
        physx::PxMaterial *pxMaterial = GetPxPhysics()->createMaterial(0.5f,
                                                                       0.5f,
                                                                       0.5f);
        pxActor = physx::PxCreateDynamic(*GetPxPhysics(),
                                      pxTransform,
                                      physx::PxSphereGeometry(3.0f),
                                      *pxMaterial,
                                      10.0f);

        pxSceneCont->m_gameObjectToPxActor.Add(phObjGo, pxActor);
        pxSceneCont->m_pxActorToGameObject.Add(pxActor, phObjGo);

        physx::PxRigidDynamic *pxRd = SCAST<physx::PxRigidDynamic*>(pxActor);
        pxSceneCont->GetPxScene()->addActor(*pxRd);
    }
    return pxActor;
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

    m_physicsObjectGatherer = new ObjectGatherer<PhysicsObject, true>();

    p_pxScene = pxScene;
    m_physicsObjectGatherer->SetRoot(scene);
    m_physicsObjectGatherer->EventEmitter<IEventsObjectGatherer>::
                         RegisterListener(this);
}

PxSceneContainer::~PxSceneContainer()
{
    p_pxScene->release();

    delete m_physicsObjectGatherer;
    for (auto &it : m_gameObjectToPxActor)
    {
        physx::PxActor *pxActor = it.second;
        pxActor->release();
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

physx::PxActor* PxSceneContainer::GetPxActorFromGameObject(GameObject *go) const
{
    if (m_gameObjectToPxActor.ContainsKey(go))
    {
        return m_gameObjectToPxActor.Get(go);
    }
    return nullptr;
}

GameObject *Physics::GetGameObjectFromPhysicsObject(PhysicsObject *phObj)
{
    if (Component *comp = DCAST<Component*>(phObj))
    {
        return comp->GetGameObject();
    }
    return nullptr;
}

GameObject *PxSceneContainer::GetGameObjectFromPxActor(physx::PxActor *pxActor) const
{
    if (m_pxActorToGameObject.ContainsKey(pxActor))
    {
        return m_pxActorToGameObject.Get(pxActor);
    }
    return nullptr;
}

void PxSceneContainer::OnObjectGathered(PhysicsObject *phObj)
{
    Physics::GetInstance()->CreateIntoPxScene(phObj);
}

void PxSceneContainer::OnObjectUnGathered(GameObject *previousGameObject,
                                          PhysicsObject *phObj)
{
    (void) phObj;
    if (physx::PxActor *prevPxActor = GetPxActorFromGameObject(previousGameObject))
    {
        m_gameObjectToPxActor.Remove(previousGameObject);
        m_pxActorToGameObject.Remove(prevPxActor);
    }
}
