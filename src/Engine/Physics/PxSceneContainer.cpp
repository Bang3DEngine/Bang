#include "Bang/PxSceneContainer.h"

#include <stdint.h>

#include "Bang/Assert.h"
#include "Bang/ClassDB.h"
#include "Bang/Collider.h"
#include "Bang/Collision.h"
#include "Bang/CollisionContact.h"
#include "Bang/Component.h"
#include "Bang/EventEmitter.h"
#include "Bang/EventListener.tcc"
#include "Bang/GameObject.h"
#include "Bang/GameObject.tcc"
#include "Bang/IEventsDestroy.h"
#include "Bang/IEventsGameObjectPhysics.h"
#include "Bang/IEventsObjectGatherer.h"
#include "Bang/Map.tcc"
#include "Bang/ObjectGatherer.h"
#include "Bang/ObjectGatherer.tcc"
#include "Bang/Physics.h"
#include "Bang/PhysicsComponent.h"
#include "Bang/RayCastHitInfo.h"
#include "Bang/RayCastInfo.h"
#include "Bang/Scene.h"
#include "PxActor.h"
#include "PxFiltering.h"
#include "PxPhysics.h"
#include "PxQueryFiltering.h"
#include "PxQueryReport.h"
#include "PxRigidActor.h"
#include "PxRigidDynamic.h"
#include "PxScene.h"
#include "PxSceneDesc.h"
#include "extensions/PxDefaultCpuDispatcher.h"
#include "foundation/PxFlags.h"
#include "foundation/PxSimpleTypes.h"
#include "foundation/PxVec3.h"

namespace Bang
{
class IEventsGameObjectPhysics;
}

namespace physx
{
class PxRigidBody;
class PxShape;
class PxTransform;
}  // namespace physx

using namespace Bang;
using namespace physx;

physx::PxFilterFlags CollisionFilterShader(
    physx::PxFilterObjectAttributes attributes0,
    physx::PxFilterData filterData0,
    physx::PxFilterObjectAttributes attributes1,
    physx::PxFilterData filterData1,
    physx::PxPairFlags &retPairFlags,
    const void *constantBlock,
    PxU32 constantBlockSize)
{
    BANG_UNUSED_4(filterData0, filterData1, constantBlock, constantBlockSize);

    retPairFlags =
        PxPairFlag::eSOLVE_CONTACT | PxPairFlag::eDETECT_DISCRETE_CONTACT |
        PxPairFlag::eNOTIFY_TOUCH_FOUND | PxPairFlag::eNOTIFY_TOUCH_PERSISTS |
        PxPairFlag::eNOTIFY_TOUCH_LOST | PxPairFlag::eNOTIFY_CONTACT_POINTS;

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
    sceneDesc.cpuDispatcher = PxDefaultCpuDispatcherCreate(1);
    sceneDesc.filterShader = CollisionFilterShader;
    sceneDesc.simulationEventCallback = this;

    PxScene *pxScene = ph->GetPxPhysics()->createScene(sceneDesc);
    pxScene->setFlag(PxSceneFlag::eENABLE_ACTIVE_ACTORS, true);
    pxScene->setSimulationEventCallback(this);

    m_physicsObjectGatherer = new ObjectGatherer<PhysicsComponent, true>();
    m_physicsObjectGatherer->SetRoot(scene);
    m_physicsObjectGatherer
        ->EventEmitter<IEventsObjectGatherer>::RegisterListener(this);

    p_scene = scene;
    p_pxScene = pxScene;
}

PxSceneContainer::~PxSceneContainer()
{
    if (GetPxScene())
    {
        if (PxDefaultCpuDispatcher *cpuDisp = DCAST<PxDefaultCpuDispatcher *>(
                GetPxScene()->getCpuDispatcher()))
        {
            cpuDisp->release();
        }
        GetPxScene()->release();
    }
    delete m_physicsObjectGatherer;
}

void PxSceneContainer::ResetStepTimeReference()
{
    m_lastStepTime = Time::GetNow();
}

void PxSceneContainer::ChangePxRigidActor(PxSceneContainer *psc,
                                          PhysicsComponent *phComp,
                                          PxRigidActor *newPxRigidActor)
{
    physx::PxActor *oldPxActor = phComp->GetPxRigidActor();

    GameObject *phGo = phComp->GetGameObject();
    const Array<PhysicsComponent *> descPhObjs =
        phGo->GetComponentsInDescendantsAndThis<PhysicsComponent>();

    for (PhysicsComponent *descPhObj : descPhObjs)
    {
        Component *descPhComp = DCAST<Component *>(descPhObj);
        GameObject *descPhGo = descPhComp->GetGameObject();
        if (psc)
        {
            psc->m_gameObjectToPxActor.Add(descPhGo, newPxRigidActor);
            psc->m_pxActorToGameObject.Add(newPxRigidActor, descPhGo);
        }

        if (Collider *descColl = DCAST<Collider *>(descPhObj))
        {
            if (PxShape *descPxShape = descColl->GetPxShape())
            {
                newPxRigidActor->attachShape(*descPxShape);
            }
        }
        descPhObj->SetPxRigidActor(newPxRigidActor);
    }

    if (oldPxActor)
    {
        if (psc)
        {
            psc->GetPxScene()->removeActor(*oldPxActor);
            psc->m_pxActorToGameObject.Remove(oldPxActor);
        }
        oldPxActor->release();
    }

    if (psc)
    {
        psc->GetPxScene()->addActor(*newPxRigidActor);
    }
}

Scene *PxSceneContainer::GetScene() const
{
    return p_scene;
}

PxScene *PxSceneContainer::GetPxScene() const
{
    return p_pxScene;
}

Array<Collider *> PxSceneContainer::GetColliders() const
{
    Array<Collider *> colliders;
    const Array<PhysicsComponent *> &phComps =
        m_physicsObjectGatherer->GetGatheredObjects();
    for (PhysicsComponent *phComp : phComps)
    {
        if (Collider *coll = DCAST<Collider *>(phComp))
        {
            colliders.PushBack(coll);
        }
    }
    return colliders;
}

Collider *PxSceneContainer::GetColliderFromPxShape(
    physx::PxShape *pxShape) const
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

    const PxU32 bufferSize = 256;  // [in] size of 'hitBuffer'
    PxRaycastHit
        hitArray[bufferSize];  // [out] User provided buffer for results
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
        hit.m_distance = pxRCHit.distance;
        hit.m_faceIndex = pxRCHit.faceIndex;
        hit.m_position = Physics::GetVector3FromPxVec3(pxRCHit.position);
        hit.m_normal = Physics::GetVector3FromPxVec3(pxRCHit.normal);
        hit.m_uv = Vector2(pxRCHit.u, pxRCHit.v);

        hit.p_collider = GetColliderFromPxShape(pxRCHit.shape);

        hitInfo->m_hits.PushBack(hit);
    }
}

PxActor *PxSceneContainer::GetAncestorOrThisPxActor(GameObject *go)
{
    if (go)
    {
        if (PxActor *pxActor = GetPxActorFromGameObject(go))
        {
            return pxActor;
        }
        else
        {
            return GetAncestorOrThisPxActor(go->GetParent());
        }
    }
    return nullptr;
}

PxActor *PxSceneContainer::GetPxActorFromGameObject(GameObject *go) const
{
    if (!m_gameObjectToPxActor.ContainsKey(go))
    {
        Array<PhysicsComponent *> phComps =
            go->GetComponents<PhysicsComponent>();
        for (PhysicsComponent *phComp : phComps)
        {
            if (PxRigidActor *pxRA = phComp->GetPxRigidActor())
            {
                m_gameObjectToPxActor.Add(go, pxRA);
                return pxRA;
            }
        }
    }
    else
    {
        return m_gameObjectToPxActor.Get(go);
    }
    return nullptr;
}

void PxSceneContainer::onConstraintBreak(PxConstraintInfo *constraints,
                                         PxU32 count)
{
    BANG_UNUSED_2(constraints, count);
}

void PxSceneContainer::onWake(PxActor **actors, PxU32 count)
{
    BANG_UNUSED_2(actors, count);
}

void PxSceneContainer::onSleep(PxActor **actors, PxU32 count)
{
    BANG_UNUSED_2(actors, count);
}

void PxSceneContainer::onContact(const PxContactPairHeader &pairHeader,
                                 const PxContactPair *pairs,
                                 PxU32 nbPairs)
{
    for (uint i = 0; i < nbPairs; i++)
    {
        const PxContactPair &cp = pairs[i];
        if (cp.events.isSet(PxPairFlag::eNOTIFY_TOUCH_FOUND) ||
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
                PxU32 nbContacts =
                    pairs[i].extractContacts(contacts, bufferSize);
                for (PxU32 j = 0; j < nbContacts; j++)
                {
                    CollisionContact cContact;
                    cContact.m_point =
                        Physics::GetVector3FromPxVec3(contacts[j].position);
                    cContact.m_normal =
                        Physics::GetVector3FromPxVec3(contacts[j].normal);
                    cContact.m_impulse =
                        Physics::GetVector3FromPxVec3(contacts[j].impulse);
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
                    cp.events.isSet(PxPairFlag::eNOTIFY_TOUCH_FOUND)
                        ? &EEPH::OnCollisionEnter
                        : cp.events.isSet(PxPairFlag::eNOTIFY_TOUCH_PERSISTS)
                              ? &EEPH::OnCollisionPersists
                              : &EEPH::OnCollisionExit;

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

void PxSceneContainer::onTrigger(PxTriggerPair *pairs, PxU32 nbPairs)
{
    for (uint i = 0; i < nbPairs; i++)
    {
        const PxTriggerPair &tp = pairs[i];
        if (tp.status == PxPairFlag::eNOTIFY_TOUCH_FOUND ||
            tp.status == PxPairFlag::eNOTIFY_TOUCH_LOST)
        {
            PxActor *otherPxActor = tp.otherActor;
            PxActor *triggerPxActor = tp.triggerActor;
            GameObject *otherGo = GetGameObjectFromPxActor(otherPxActor);
            GameObject *triggerGo = GetGameObjectFromPxActor(triggerPxActor);

            if (otherGo && triggerGo)
            {
                Collider *otherCollider = GetColliderFromPxShape(tp.otherShape);
                Collider *triggerCollider =
                    GetColliderFromPxShape(tp.triggerShape);

                using EEPH = EventListener<IEventsGameObjectPhysics>;
                auto triggerCallback =
                    (tp.status == PxPairFlag::eNOTIFY_TOUCH_FOUND)
                        ? &EEPH::OnTriggerEnter
                        : &EEPH::OnTriggerExit;

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

void PxSceneContainer::onAdvance(const PxRigidBody *const *bodyBuffer,
                                 const PxTransform *poseBuffer,
                                 const PxU32 count)
{
    BANG_UNUSED_3(bodyBuffer, poseBuffer, count);
}

void PxSceneContainer::OnObjectGathered(PhysicsComponent *phComp)
{
    Physics *ph = Physics::GetInstance();
    ASSERT(ph);

    GameObject *phCompGo = phComp->GetGameObject();
    ASSERT(phCompGo);

    Array<PhysicsComponent *> phCompsInDescendants =
        phCompGo->GetComponentsInDescendantsAndThis<PhysicsComponent>();

    // Does this gameObject need a pxActor for him to be created?
    // You need to create a pxActor when you hold a PhysicsComponent component,
    // but neither do you or your ancestors
    ASSERT(phCompGo->HasComponent<PhysicsComponent>());

    PxRigidActor *pxRA =
        SCAST<PxRigidActor *>(GetAncestorOrThisPxActor(phCompGo));
    if (!pxRA)
    {
        ASSERT(!GetPxActorFromGameObject(phCompGo));
        pxRA = ph->CreateNewPxRigidActor(phComp->GetStatic(),
                                         phCompGo->GetTransform());
        phComp->SetPxRigidActor(pxRA);
    }
    ASSERT(pxRA);

    if (!m_gameObjectToPxActor.ContainsKey(phCompGo))
    {
        m_gameObjectToPxActor.Add(phCompGo, pxRA);
    }

    if (!m_pxActorToGameObject.ContainsKey(pxRA))
    {
        m_pxActorToGameObject.Add(pxRA, phCompGo);
        GetPxScene()->addActor(*pxRA);
    }
    ASSERT(GetPxActorFromGameObject(phCompGo));
    ASSERT(m_pxActorToGameObject.ContainsKey(pxRA));

    // For each physics object in descendants, update its pxActor
    for (PhysicsComponent *phComp : phCompsInDescendants)
    {
        bool forceKinematic = false;
        if (phComp->GetPhysicsComponentType() !=
            PhysicsComponent::Type::RIGIDBODY)
        {
            // Collider
            Collider *collider = SCAST<Collider *>(phComp);
            if (collider->GetPxShape())
            {
                m_pxShapeToCollider.Add(collider->GetPxShape(), collider);
            }

            forceKinematic = (phComp->GetPhysicsComponentType() ==
                              PhysicsComponent::Type::MESH_COLLIDER);
        }

        if (forceKinematic)
        {
            if (PxRigidDynamic *pxRD = phComp->GetPxRigidDynamic())
            {
                pxRD->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
            }
        }
        phComp->SetPxRigidActor(pxRA);

        Component *comp = DCAST<Component *>(phComp);
        comp->EventEmitter<IEventsDestroy>::RegisterListener(this);
    }

    // Does this gameObject need to release the pxActor if it had one?
    if (PxActor *pxActor = GetPxActorFromGameObject(phCompGo))
    {
        Array<PhysicsComponent *> phComps =
            phCompGo->GetComponentsInDescendantsAndThis<PhysicsComponent>();
        if (phComps.Size() == 0)
        {
            pxActor->release();
            m_gameObjectToPxActor.Remove(phCompGo);
            m_pxActorToGameObject.Remove(pxActor);
        }
    }
}

void PxSceneContainer::OnObjectUnGathered(GameObject *prevGo,
                                          PhysicsComponent *phComp)
{
    BANG_UNUSED(prevGo);
    phComp->SetPxRigidActor(nullptr);
}

void PxSceneContainer::OnDestroyed(EventEmitter<IEventsDestroy> *ee)
{
    if (PhysicsComponent *phComp = DCAST<PhysicsComponent *>(ee))
    {
        switch (phComp->GetPhysicsComponentType())
        {
            case PhysicsComponent::Type::RIGIDBODY:
            {
                // RigidBody *rb = SCAST<RigidBody*>(phComp);
            }
            break;

            case PhysicsComponent::Type::BOX_COLLIDER:
            case PhysicsComponent::Type::SPHERE_COLLIDER:
            case PhysicsComponent::Type::CAPSULE_COLLIDER:
            {
                // Collider *coll = SCAST<Collider*>(phComp);
                // if (coll->GetPxShape()->isReleasable())
                // {
                //     coll->GetPxShape()->release();
                // }
            }
            break;

            default: break;
        }
    }
}
