#include "Bang/Physics.h"

#include "PxPhysicsAPI.h"

#include "Bang/Scene.h"
#include "Bang/RigidBody.h"
#include "Bang/Transform.h"
#include "Bang/Application.h"
#include "Bang/BoxCollider.h"
#include "Bang/PhysicsObject.h"
#include "Bang/SphereCollider.h"

using namespace physx;
USING_NAMESPACE_BANG

const float Physics::MaximumSleepTime = 0.1f;

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
        PxScene *pxScene = pxSceneContainer->GetPxScene();
        ASSERT(pxScene);

        const auto &allPhObjs = pxSceneContainer->m_physicsObjectGatherer->GetList();
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

void Physics::Step(Scene *scene, float simulationTime_)
{
    PxSceneContainer *pxSceneContainer = GetPxSceneContainerFromScene(scene);
    ASSERT(pxSceneContainer);

    PxScene *pxScene = pxSceneContainer->GetPxScene();
    ASSERT(pxScene);

    // Step
    float simulationTime = Math::Min(MaximumSleepTime, simulationTime_);
    ResetStepTimeReference(scene);
    pxScene->simulate(simulationTime);
    pxScene->fetchResults(true);

    uint32_t numActActorsOut;
    PxActor** activeActors = pxScene->getActiveActors(numActActorsOut);
    for (uint32_t i = 0; i < numActActorsOut; ++i)
    {
        PxActor *pxActor = activeActors[i];
        GameObject *go = pxSceneContainer->GetGameObjectFromPxActor(pxActor);

        List<PhysicsObject*> physicsObjects = go->GetComponents<PhysicsObject>();
        for (PhysicsObject *phObj : physicsObjects)
        {
            switch (phObj->GetPhysicsObjectType())
            {
                case PhysicsObject::Type::RIGIDBODY:
                {
                    // RigidBody *rb = SCAST<RigidBody*>(phObj);
                    PxRigidBody *pxRB = SCAST<PxRigidBody*>(pxActor);
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
    if (secondsSinceLastStep >= GetSleepStepTimeSeconds())
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

PxActor* Physics::CreateIntoPxScene(PhysicsObject *phObj)
{
    GameObject *phObjGo = Physics::GetGameObjectFromPhysicsObject(phObj);
    ASSERT(phObjGo);

    Scene *scene = GetSceneFromPhysicsObject(phObj);
    ASSERT(scene);

    PxActor *pxActor = nullptr;
    if (PxSceneContainer *pxSceneCont = GetPxSceneContainerFromScene(scene))
    {
        pxActor = pxSceneCont->GetPxActorFromGameObject(phObjGo);
        if (!pxActor)
        {
            PxTransform pxTransform = GetPxTransformFromTransform(
                                                      phObjGo->GetTransform());

            // Create pxActor
            PxMaterial *pxMaterial = CreateNewMaterial();
            PxRigidDynamic *pxRD = PxCreateDynamic(*GetPxPhysics(),
                                                   pxTransform,
                                                   PxSphereGeometry(0.01f),
                                                   *pxMaterial,
                                                   10.0f);
            pxActor = pxRD;

            pxRD->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
            pxSceneCont->m_gameObjectToPxActor.Add(phObjGo, pxActor);
            pxSceneCont->m_pxActorToGameObject.Add(pxActor, phObjGo);

            PxShape *shape;
            pxRD->getShapes(&shape, 1);
            pxRD->detachShape(*shape);

            pxSceneCont->GetPxScene()->addActor(*pxActor);
        }

        switch (phObj->GetPhysicsObjectType())
        {
            case PhysicsObject::Type::RIGIDBODY:
            {
                PxRigidDynamic *pxRD = SCAST<PxRigidDynamic*>(pxActor);

                RigidBody *rb = SCAST<RigidBody*>(phObj);
                rb->SetPxRigidDynamic(pxRD);

                pxRD->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, false);
            }
            break;

            case PhysicsObject::Type::BOX_COLLIDER:
            {
                BoxCollider *bc = SCAST<BoxCollider*>(phObj);

                PxMaterial *pxMaterial = CreateNewMaterial();
                PxRigidDynamic *pxRigidDynamic = SCAST<PxRigidDynamic*>(pxActor);

                PxBoxGeometry pxBoxGeom = PxBoxGeometry(0.1f, 0.1f, 0.1f);
                PxShape *pxBoxShape = pxRigidDynamic->createShape(pxBoxGeom, *pxMaterial);
                bc->SetPxRigidBody( pxRigidDynamic );
                bc->SetPxShape( pxBoxShape );
                bc->UpdateShapeGeometry();
            }
            break;

            case PhysicsObject::Type::SPHERE_COLLIDER:
            {
                SphereCollider *sc = SCAST<SphereCollider*>(phObj);

                PxMaterial *pxMaterial = CreateNewMaterial();
                PxRigidDynamic *pxRigidDynamic = SCAST<PxRigidDynamic*>(pxActor);

                PxSphereGeometry pxSphereGeom = PxSphereGeometry(0.1f);
                PxShape *pxSphereShape =
                        pxRigidDynamic->createShape(pxSphereGeom, *pxMaterial);
                sc->SetPxRigidBody( pxRigidDynamic );
                sc->SetPxShape( pxSphereShape );
                sc->UpdateShapeGeometry();
            }
            break;

            default: break;
        }
    }
    return pxActor;
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
        transform->SetLocalPosition( GetVector3FromPxVec3(pxTransform.p) );
        transform->SetLocalRotation( GetQuaternionFromPxQuat(pxTransform.q) );
    }
}

PxTransform Physics::GetPxTransformFromTransform(Transform *tr)
{
    PxTransform pxTransform;
    if (tr)
    {
        pxTransform.p = Physics::GetPxVec3FromVector3( tr->GetPosition() );
        pxTransform.q = Physics::GetPxQuatFromQuaternion( tr->GetRotation() );
    }
    return pxTransform;
}

// PxSceneContainer
PxSceneContainer::PxSceneContainer(Scene *scene)
{
    Physics *ph = Physics::GetInstance();

    PxSceneDesc sceneDesc(ph->GetPxPhysics()->getTolerancesScale());
    sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
    sceneDesc.cpuDispatcher	= PxDefaultCpuDispatcherCreate(2);
    sceneDesc.filterShader	= PxDefaultSimulationFilterShader;

    PxScene *pxScene = ph->GetPxPhysics()->createScene(sceneDesc);
    pxScene->setFlag(PxSceneFlag::eENABLE_ACTIVE_ACTORS, true);

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
        PxActor *pxActor = it.second;
        pxActor->release();
    }
}

void PxSceneContainer::ResetStepTimeReference()
{
    m_lastStepTimeMillis = Time::GetNow_Millis();
}

PxScene *PxSceneContainer::GetPxScene() const
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

PxActor* PxSceneContainer::GetPxActorFromGameObject(GameObject *go) const
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

GameObject *PxSceneContainer::GetGameObjectFromPxActor(PxActor *pxActor) const
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
    if (PxActor *prevPxActor = GetPxActorFromGameObject(previousGameObject))
    {
        m_gameObjectToPxActor.Remove(previousGameObject);
        m_pxActorToGameObject.Remove(prevPxActor);
    }
}
