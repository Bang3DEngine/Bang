#include "Bang/PhysicsObject.h"

USING_NAMESPACE_BANG

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
        p_pxRigidDynamic = pxRigidDynamic;
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

void PhysicsObject::OnPxRigidDynamicSet()
{
    // Empty;
}

