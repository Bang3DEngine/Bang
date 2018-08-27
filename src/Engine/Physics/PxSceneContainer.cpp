#include "Bang/PxSceneContainer.h"

#include "Bang/Scene.h"
#include "Bang/Physics.h"
#include "Bang/Collider.h"
#include "Bang/Collision.h"
#include "Bang/RigidBody.h"
#include "Bang/GameObject.h"
#include "Bang/BoxCollider.h"
#include "Bang/SphereCollider.h"
#include "Bang/CapsuleCollider.h"
#include "Bang/IEventsGameObjectPhysics.h"

USING_NAMESPACE_BANG

using namespace physx;

physx::PxFilterFlags CollisionFilterShader(
                            physx::PxFilterObjectAttributes attributes0,
                            physx::PxFilterData filterData0,
                            physx::PxFilterObjectAttributes attributes1,
                            physx::PxFilterData filterData1,
                            physx::PxPairFlags& retPairFlags,
                            const void *constantBlock,
                            PxU32 constantBlockSize)
{
    (void) attributes0;
    (void) filterData0;
    (void) attributes1;
    (void) filterData1;
    (void) constantBlock;
    (void) constantBlockSize;

    retPairFlags = PxPairFlag::eSOLVE_CONTACT |
                   PxPairFlag::eDETECT_DISCRETE_CONTACT |
                   PxPairFlag::eNOTIFY_TOUCH_FOUND |
                   PxPairFlag::eNOTIFY_TOUCH_PERSISTS |
                   PxPairFlag::eNOTIFY_TOUCH_LOST |
                   PxPairFlag::eNOTIFY_CONTACT_POINTS;

    if (PxFilterObjectIsKinematic(attributes0) &&
        PxFilterObjectIsKinematic(attributes1))
    {
        retPairFlags.clear(PxPairFlag::eSOLVE_CONTACT);
    }

    if (PxFilterObjectIsTrigger(attributes0) &&
        PxFilterObjectIsTrigger(attributes1))
    {
        retPairFlags.clear(PxPairFlag::eNOTIFY_TOUCH_PERSISTS);
    }

    PxFilterFlags filterFlags;
    filterFlags.set(PxFilterFlag::eNOTIFY);
    filterFlags.clear(PxFilterFlag::eCALLBACK);
    return filterFlags;
}

PxSceneContainer::PxSceneContainer(Scene *scene)
{
    Physics *ph = Physics::GetInstance();

    PxSceneDesc sceneDesc(ph->GetPxPhysics()->getTolerancesScale());
    sceneDesc.gravity = Physics::GetPxVec3FromVector3(ph->GetGravity());
    sceneDesc.cpuDispatcher	= PxDefaultCpuDispatcherCreate(2);
    sceneDesc.simulationEventCallback = this;
    sceneDesc.filterShader	= CollisionFilterShader;

    PxScene *pxScene = ph->GetPxPhysics()->createScene(sceneDesc);
    pxScene->setFlag(PxSceneFlag::eENABLE_ACTIVE_ACTORS, true);
    pxScene->setSimulationEventCallback(this);

    m_physicsObjectGatherer = new ObjectGatherer<PhysicsObject, true>();

    p_pxScene = pxScene;
    m_physicsObjectGatherer->SetRoot(scene);
    m_physicsObjectGatherer->EventEmitter<IEventsObjectGatherer>::
                             RegisterListener(this);
}

PxSceneContainer::~PxSceneContainer()
{
    GetPxScene()->release();

    delete m_physicsObjectGatherer;

    for (auto &it : m_gameObjectToPxActor)
    {
        PxActor *pxActor = it.second;
        pxActor->release();
    }
    m_gameObjectToPxActor.Clear();
    m_pxActorToGameObject.Clear();

    // for (auto &it : m_pxShapeToCollider)
    // {
    //     PxShape *pxShape = it.first;
    //     pxShape->release();
    // }
    m_pxShapeToCollider.Clear();
}

void PxSceneContainer::ResetStepTimeReference()
{
    m_lastStepTimeMillis = Time::GetNow_Millis();
}

PxScene *PxSceneContainer::GetPxScene() const
{
    return p_pxScene;
}

Collider* PxSceneContainer::GetColliderFromPxShape(physx::PxShape *pxShape) const
{
    if (m_pxShapeToCollider.ContainsKey(pxShape))
    {
        return m_pxShapeToCollider.Get(pxShape);
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

void PxSceneContainer::RayCast(const RayCastInfo &rcInfo,
                               RayCastHitInfo *hitInfo)
{
    if (!hitInfo)
    {
        return;
    }

    PxScene *pxScene = GetPxScene();
    Vector3 unitDir = rcInfo.direction.NormalizedSafe();

    const PxU32 bufferSize = 256;        // [in] size of 'hitBuffer'
    PxRaycastHit hitArray[bufferSize];  // [out] User provided buffer for results
    PxRaycastBuffer hitOutBuffer(hitArray, bufferSize);

    PxHitFlags hf = (PxHitFlag::eDEFAULT | PxHitFlag::eUV);
    PxQueryFilterData fd;
    fd.flags = (PxQueryFlag::eNO_BLOCK | PxQueryFlag::eDYNAMIC);

    pxScene->raycast(Physics::GetPxVec3FromVector3(rcInfo.origin),
                     Physics::GetPxVec3FromVector3(unitDir),
                     rcInfo.maxDistance,
                     hitOutBuffer,
                     hf,
                     fd);

    for (uint32_t i = 0; i < hitOutBuffer.getNbTouches(); ++i)
    {
        const PxRaycastHit &pxRCHit = hitOutBuffer.getTouch(i);

        RayCastHit hit;
        hit.m_distance  = pxRCHit.distance;
        hit.m_faceIndex = pxRCHit.faceIndex;
        hit.m_position  = Physics::GetVector3FromPxVec3(pxRCHit.position);
        hit.m_normal    = Physics::GetVector3FromPxVec3(pxRCHit.normal);
        hit.m_uv        = Vector2(pxRCHit.u, pxRCHit.v);

        hit.p_collider  = GetColliderFromPxShape( pxRCHit.shape );

        hitInfo->m_hits.PushBack( hit );
    }
}

PxActor* PxSceneContainer::GetPxActorFromGameObject(GameObject *go) const
{
    if (m_gameObjectToPxActor.ContainsKey(go))
    {
        return m_gameObjectToPxActor.Get(go);
    }
    return nullptr;
}

void PxSceneContainer::ReleasePxActorIfNoMorePhysicsObjectsOnIt(GameObject *go)
{
    if (PxActor *pxActor = GetPxActorFromGameObject(go))
    {
        Array<PhysicsObject*> phObjs = go->GetComponents<PhysicsObject>();
        if (phObjs.Size() == 0)
        {
            pxActor->release();
            m_gameObjectToPxActor.Remove(go);
            m_pxActorToGameObject.Remove(pxActor);
        }
    }
}

void PxSceneContainer::onConstraintBreak(PxConstraintInfo* constraints,
                                         PxU32 count)
{
    (void) constraints;
    (void) count;
}

void PxSceneContainer::onWake(PxActor** actors, PxU32 count)
{
    (void) actors;
    (void) count;
}

void PxSceneContainer::onSleep(PxActor** actors, PxU32 count)
{
    (void) actors;
    (void) count;
}

void PxSceneContainer::onContact(const PxContactPairHeader& pairHeader,
                                 const PxContactPair* pairs,
                                 PxU32 nbPairs)
{
    for (uint i = 0; i < nbPairs; i++)
    {
        const PxContactPair& cp = pairs[i];
        if(cp.events.isSet(PxPairFlag::eNOTIFY_TOUCH_FOUND) ||
           cp.events.isSet(PxPairFlag::eNOTIFY_TOUCH_PERSISTS) ||
           cp.events.isSet(PxPairFlag::eNOTIFY_TOUCH_LOST))
        {
            PxActor *pxActor0 = pairHeader.actors[0];
            PxActor *pxActor1 = pairHeader.actors[1];
            GameObject *go0 = GetGameObjectFromPxActor(pxActor0);
            GameObject *go1 = GetGameObjectFromPxActor(pxActor1);

            if (go0 && go1)
            {
                Collider *collider0 = GetColliderFromPxShape(cp.shapes[0]);
                Collider *collider1 = GetColliderFromPxShape(cp.shapes[1]);

                constexpr PxU32 bufferSize = 64;
                PxContactPairPoint contacts[bufferSize];

                Collision collisionFor0;
                Collision collisionFor1;
                PxU32 nbContacts = pairs[i].extractContacts(contacts, bufferSize);
                for(PxU32 j = 0; j < nbContacts; j++)
                {
                    CollisionContact cContact;
                    cContact.m_point = Physics::GetVector3FromPxVec3(
                                                        contacts[j].position);
                    cContact.m_normal = Physics::GetVector3FromPxVec3(
                                                        contacts[j].normal);
                    cContact.m_impulse = Physics::GetVector3FromPxVec3(
                                                        contacts[j].impulse);
                    cContact.m_separation = contacts[j].separation;

                    CollisionContact cContactFor0 = cContact;
                    cContactFor0.p_otherCollider = collider1;

                    CollisionContact cContactFor1 = cContact;
                    cContactFor1.p_otherCollider = collider0;

                    collisionFor0.m_contacts.PushBack(cContactFor0);
                    collisionFor1.m_contacts.PushBack(cContactFor1);
                }

                using EEPH = EventListener<IEventsGameObjectPhysics>;
                auto collisionCallback =
                        cp.events.isSet(PxPairFlag::eNOTIFY_TOUCH_FOUND) ?
                           &EEPH::OnCollisionEnter :
                        cp.events.isSet(PxPairFlag::eNOTIFY_TOUCH_PERSISTS) ?
                           &EEPH::OnCollisionPersists : &EEPH::OnCollisionExit;

                for (EEPH *listener : go0->GetComponents<EEPH>())
                {
                    (listener->*collisionCallback)(collisionFor0);
                }
                for (EEPH *listener : go1->GetComponents<EEPH>())
                {
                    (listener->*collisionCallback)(collisionFor1);
                }
            }
        }
    }
}

void PxSceneContainer::onTrigger(PxTriggerPair* pairs, PxU32 nbPairs)
{
    for (uint i = 0; i < nbPairs; i++)
    {
        const PxTriggerPair& tp = pairs[i];
        if (tp.status == PxPairFlag::eNOTIFY_TOUCH_FOUND ||
            tp.status == PxPairFlag::eNOTIFY_TOUCH_LOST)
        {
            PxActor *otherPxActor   = tp.otherActor;
            PxActor *triggerPxActor = tp.triggerActor;
            GameObject *otherGo   = GetGameObjectFromPxActor(otherPxActor);
            GameObject *triggerGo = GetGameObjectFromPxActor(triggerPxActor);

            if (otherGo && triggerGo)
            {
                Collider *otherCollider   = GetColliderFromPxShape(tp.otherShape);
                Collider *triggerCollider = GetColliderFromPxShape(tp.triggerShape);

                using EEPH = EventListener<IEventsGameObjectPhysics>;
                auto triggerCallback =
                    (tp.status == PxPairFlag::eNOTIFY_TOUCH_FOUND) ?
                        &EEPH::OnTriggerEnter : &EEPH::OnTriggerExit;

                for (auto listener : otherGo->GetComponents<EEPH>())
                {
                    (listener->*triggerCallback)(triggerCollider);
                }
                for (auto listener : triggerGo->GetComponents<EEPH>())
                {
                    (listener->*triggerCallback)(otherCollider);
                }
            }
        }
    }
}

void PxSceneContainer::onAdvance(const PxRigidBody * const * bodyBuffer,
                                 const PxTransform* poseBuffer,
                                 const PxU32 count)
{
    (void) bodyBuffer;
    (void) poseBuffer;
    (void) count;
}

void PxSceneContainer::OnObjectGathered(PhysicsObject *phObj)
{
    Physics *ph = Physics::GetInstance();

    GameObject *phObjGo = Physics::GetGameObjectFromPhysicsObject(phObj);
    ASSERT(phObjGo);

    PxActor *pxActor = GetPxActorFromGameObject(phObjGo);
    if (!pxActor)
    {
        PxTransform pxTransform = Physics::GetPxTransformFromTransform(
                                                  phObjGo->GetTransform());

        // Create pxActor
        PxMaterial *pxMaterial = ph->CreateNewMaterial();
        PxRigidDynamic *pxRD = PxCreateDynamic(*Physics::GetInstance()->GetPxPhysics(),
                                               pxTransform,
                                               PxSphereGeometry(0.01f),
                                               *pxMaterial,
                                               10.0f);
        pxActor = pxRD;

        pxRD->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
        m_gameObjectToPxActor.Add(phObjGo, pxActor);
        m_pxActorToGameObject.Add(pxActor, phObjGo);

        PxShape *shape;
        pxRD->getShapes(&shape, 1);
        pxRD->detachShape(*shape);

        GetPxScene()->addActor(*pxActor);
    }

    PxShape *createdPxShape = nullptr;
    switch (phObj->GetPhysicsObjectType())
    {
        case PhysicsObject::Type::RIGIDBODY:
        {
            PxRigidDynamic *pxRD = SCAST<PxRigidDynamic*>(pxActor);

            RigidBody *rb = SCAST<RigidBody*>(phObj);
            rb->SetPxRigidDynamic(pxRD);

            pxRD->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, false);
            rb->UpdatePxRigidDynamicValues();
        }
        break;

        case PhysicsObject::Type::BOX_COLLIDER:
        {
            BoxCollider *bc = SCAST<BoxCollider*>(phObj);

            PxMaterial *pxMaterial = ph->CreateNewMaterial();
            PxRigidDynamic *pxRigidDynamic = SCAST<PxRigidDynamic*>(pxActor);

            PxBoxGeometry pxBoxGeom = PxBoxGeometry(0.1f, 0.1f, 0.1f);
            PxShape *pxBoxShape = pxRigidDynamic->createShape(pxBoxGeom, *pxMaterial);
            bc->SetPxRigidBody( pxRigidDynamic );
            bc->SetPxShape( pxBoxShape );
            bc->UpdatePxShape();
            createdPxShape = pxBoxShape;
        }
        break;

        case PhysicsObject::Type::SPHERE_COLLIDER:
        {
            SphereCollider *sc = SCAST<SphereCollider*>(phObj);

            PxMaterial *pxMaterial = ph->CreateNewMaterial();
            PxRigidDynamic *pxRigidDynamic = SCAST<PxRigidDynamic*>(pxActor);

            PxSphereGeometry pxSphereGeom = PxSphereGeometry(0.1f);
            PxShape *pxSphereShape =
                    pxRigidDynamic->createShape(pxSphereGeom, *pxMaterial);
            sc->SetPxRigidBody( pxRigidDynamic );
            sc->SetPxShape( pxSphereShape );
            sc->UpdatePxShape();
            createdPxShape = pxSphereShape;
        }
        break;

        case PhysicsObject::Type::CAPSULE_COLLIDER:
        {
            CapsuleCollider *cc = SCAST<CapsuleCollider*>(phObj);

            PxMaterial *pxMaterial = ph->CreateNewMaterial();
            PxRigidDynamic *pxRigidDynamic = SCAST<PxRigidDynamic*>(pxActor);

            PxCapsuleGeometry pxCapsuleGeom = PxCapsuleGeometry(0.1f, 0.1f);
            PxShape *pxCapsuleShape =
                    pxRigidDynamic->createShape(pxCapsuleGeom, *pxMaterial);
            cc->SetPxRigidBody( pxRigidDynamic );
            cc->SetPxShape( pxCapsuleShape );
            cc->UpdatePxShape();
            createdPxShape = pxCapsuleShape;
        }
        break;

        default: break;
    }


    if (phObj->GetPhysicsObjectType() == PhysicsObject::Type::BOX_COLLIDER    ||
        phObj->GetPhysicsObjectType() == PhysicsObject::Type::SPHERE_COLLIDER ||
        phObj->GetPhysicsObjectType() == PhysicsObject::Type::CAPSULE_COLLIDER)
    {
        ASSERT(createdPxShape);
        Collider *collider = DCAST<Collider*>(phObj);
        ASSERT(collider);
        m_pxShapeToCollider.Add(createdPxShape, collider);
    }

    if (Component *comp = DCAST<Component*>(phObj))
    {
        comp->EventEmitter<IEventsDestroy>::RegisterListener(this);
    }
}

void PxSceneContainer::OnObjectUnGathered(GameObject *previousGameObject,
                                          PhysicsObject *phObj)
{
    if (PxActor *prevPxActor = GetPxActorFromGameObject(previousGameObject))
    {
        switch (phObj->GetPhysicsObjectType())
        {
            case PhysicsObject::Type::RIGIDBODY:
            {
            }
            break;

            case PhysicsObject::Type::BOX_COLLIDER:
            case PhysicsObject::Type::SPHERE_COLLIDER:
            case PhysicsObject::Type::CAPSULE_COLLIDER:
            {
                Collider *coll = SCAST<Collider*>(phObj);
                PxRigidActor *pxRD = SCAST<PxRigidActor*>(prevPxActor);
                ASSERT(prevPxActor == coll->GetPxRigidBody());
                pxRD->detachShape(*coll->GetPxShape());
            }
            break;

            default:
            break;
        }

        ReleasePxActorIfNoMorePhysicsObjectsOnIt(previousGameObject);
    }
}

void PxSceneContainer::OnDestroyed(EventEmitter<IEventsDestroy> *ee)
{
    if (PhysicsObject *phObj = DCAST<PhysicsObject*>(ee))
    {
        switch (phObj->GetPhysicsObjectType())
        {
            case PhysicsObject::Type::RIGIDBODY:
            {
                // RigidBody *rb = SCAST<RigidBody*>(phObj);
            }
            break;

            case PhysicsObject::Type::BOX_COLLIDER:
            case PhysicsObject::Type::SPHERE_COLLIDER:
            case PhysicsObject::Type::CAPSULE_COLLIDER:
            {
                // Collider *coll = SCAST<Collider*>(phObj);
                // if (coll->GetPxShape()->isReleasable())
                // {
                //     coll->GetPxShape()->release();
                // }
            }
            break;

            default:
            break;
        }
    }
}
