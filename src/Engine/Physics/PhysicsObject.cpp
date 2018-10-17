#include "Bang/PhysicsObject.h"

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

void PhysicsObject::SetPxRigidDynamic(physx::PxRigidDynamic *pxRigidDynamic)
{
    if (pxRigidDynamic != GetPxRigidDynamic())
    {
        physx::PxRigidDynamic *prevPxRD = GetPxRigidDynamic();

        p_pxRigidDynamic = pxRigidDynamic;

        OnPxRigidDynamicChanged(prevPxRD, GetPxRigidDynamic());
    }
}

physx::PxRigidDynamic *PhysicsObject::GetPxRigidDynamic() const
{
    return p_pxRigidDynamic;
}

PhysicsObject::Type PhysicsObject::GetPhysicsObjectType() const
{
    return m_physicsObjectType;
}

void PhysicsObject::OnPxRigidDynamicChanged(
    physx::PxRigidDynamic *prevPxRigidDynamic,
    physx::PxRigidDynamic *newPxRigidDynamic)
{
    BANG_UNUSED_2(prevPxRigidDynamic, newPxRigidDynamic);
}
