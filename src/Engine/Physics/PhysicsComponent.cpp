#include "Bang/PhysicsComponent.h"

#include "Bang/Component.h"
#include "Bang/GameObject.h"
#include "Bang/Physics.h"
#include "Bang/PxSceneContainer.h"
#include "Bang/Scene.h"
#include "PxRigidDynamic.h"
#include "PxRigidStatic.h"

using namespace Bang;

PhysicsComponent::PhysicsComponent()
{
}

PhysicsComponent::~PhysicsComponent()
{
}

void PhysicsComponent::OnStart()
{
    Component::OnStart();
    SetPxEnabled(IsEnabledRecursively());
}

void PhysicsComponent::OnUpdate()
{
    Component::OnUpdate();

    // Enable/disable actor and shapes if needed
    bool enabled = IsEnabledRecursively();
    if (m_previousEnabled != enabled)
    {
        SetPxEnabled(enabled);
        m_previousEnabled = enabled;
    }
}

void PhysicsComponent::SetPhysicsComponentType(
    PhysicsComponent::Type physicsObjectType)
{
    m_physicsObjectType = physicsObjectType;
}

void PhysicsComponent::SetPxRigidActor(physx::PxRigidActor *pxRigidActor)
{
    if (pxRigidActor != GetPxRigidActor())
    {
        physx::PxRigidActor *prevPxRA = GetPxRigidActor();

        p_pxRigidActor = pxRigidActor;

        OnPxRigidActorChanged(prevPxRA, GetPxRigidActor());
    }
}

void PhysicsComponent::SetStatic(bool isStatic)
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

bool PhysicsComponent::GetStatic() const
{
    return m_static;
}

physx::PxRigidActor *PhysicsComponent::GetPxRigidActor() const
{
    return p_pxRigidActor;
}

physx::PxRigidStatic *PhysicsComponent::GetPxRigidStatic() const
{
    return GetPxRigidActor() ? DCAST<physx::PxRigidStatic *>(GetPxRigidActor())
                             : nullptr;
}

physx::PxRigidDynamic *PhysicsComponent::GetPxRigidDynamic() const
{
    return GetPxRigidActor() ? DCAST<physx::PxRigidDynamic *>(GetPxRigidActor())
                             : nullptr;
}

PhysicsComponent::Type PhysicsComponent::GetPhysicsComponentType() const
{
    return m_physicsObjectType;
}

void PhysicsComponent::OnPxRigidActorChanged(physx::PxRigidActor *prevPxDynamic,
                                             physx::PxRigidActor *newPxDynamic)
{
    BANG_UNUSED_2(prevPxDynamic, newPxDynamic);
}
