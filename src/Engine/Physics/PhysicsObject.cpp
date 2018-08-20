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

PhysicsObject::Type PhysicsObject::GetPhysicsObjectType() const
{
    return m_physicsObjectType;
}

