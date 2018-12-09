#include "Bang/PhysicsObject.h"

#include "Bang/Component.h"
#include "Bang/GameObject.h"
#include "Bang/Physics.h"
#include "Bang/PxSceneContainer.h"
#include "Bang/Scene.h"
#include "PxRigidDynamic.h"
#include "PxRigidStatic.h"

using namespace Bang;

PhysicsObject::PhysicsObject()
{
}

PhysicsObject::~PhysicsObject()
{
}

void PhysicsObject::SetPhysicsObjectType(PhysicsObject::Type physicsObjectType)
{
    m_physicsObjectType = physicsObjectType;
}

void PhysicsObject::SetPxRigidActor(physx::PxRigidActor *pxRigidActor)
{
    if (pxRigidActor != GetPxRigidActor())
    {
        physx::PxRigidActor *prevPxRA = GetPxRigidActor();

        p_pxRigidActor = pxRigidActor;

        OnPxRigidActorChanged(prevPxRA, GetPxRigidActor());
    }
}

void PhysicsObject::SetStatic(bool isStatic)
{
    if (isStatic != GetStatic())
    {
        m_static = isStatic;

        Physics *ph = Physics::GetInstance();
        physx::PxRigidActor *newPxRigidActor =
            ph->CreateNewPxRigidActor(GetStatic());

        Component *comp = DCAST<Component *>(this);
        PxSceneContainer *pxSceneContainer =
            ph->GetPxSceneContainerFromScene(comp->GetGameObject()->GetScene());
        PxSceneContainer::ChangePxRigidActor(
            pxSceneContainer, this, newPxRigidActor);
    }
}

bool PhysicsObject::GetStatic() const
{
    return m_static;
}

physx::PxRigidActor *PhysicsObject::GetPxRigidActor() const
{
    return p_pxRigidActor;
}

physx::PxRigidStatic *PhysicsObject::GetPxRigidStatic() const
{
    return GetPxRigidActor() ? DCAST<physx::PxRigidStatic *>(GetPxRigidActor())
                             : nullptr;
}

physx::PxRigidDynamic *PhysicsObject::GetPxRigidDynamic() const
{
    return GetPxRigidActor() ? DCAST<physx::PxRigidDynamic *>(GetPxRigidActor())
                             : nullptr;
}

PhysicsObject::Type PhysicsObject::GetPhysicsObjectType() const
{
    return m_physicsObjectType;
}

void PhysicsObject::OnPxRigidActorChanged(physx::PxRigidActor *prevPxDynamic,
                                          physx::PxRigidActor *newPxDynamic)
{
    BANG_UNUSED_2(prevPxDynamic, newPxDynamic);
}
