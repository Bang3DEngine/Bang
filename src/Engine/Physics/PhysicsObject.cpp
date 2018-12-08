#include "Bang/PhysicsObject.h"

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

void PhysicsObject::SetPxActor(physx::PxActor *pxActor)
{
    if (pxActor != GetPxActor())
    {
        physx::PxActor *prevPxRD = GetPxActor();

        p_pxActor = pxActor;

        OnPxActorChanged(prevPxRD, GetPxActor());
    }
}

physx::PxActor *PhysicsObject::GetPxActor() const
{
    return p_pxActor;
}

physx::PxRigidStatic *PhysicsObject::GetPxRigidStatic() const
{
    return GetPxActor() ? DCAST<physx::PxRigidStatic *>(GetPxActor()) : nullptr;
}

physx::PxRigidDynamic *PhysicsObject::GetPxRigidDynamic() const
{
    return GetPxActor() ? DCAST<physx::PxRigidDynamic *>(GetPxActor())
                        : nullptr;
}

PhysicsObject::Type PhysicsObject::GetPhysicsObjectType() const
{
    return m_physicsObjectType;
}

void PhysicsObject::OnPxActorChanged(physx::PxActor *prevPxDynamic,
                                     physx::PxActor *newPxDynamic)
{
    BANG_UNUSED_2(prevPxDynamic, newPxDynamic);
}
