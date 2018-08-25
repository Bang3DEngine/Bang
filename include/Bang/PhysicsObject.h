#ifndef PHYSICSOBJECT_H
#define PHYSICSOBJECT_H

#include "Bang/Bang.h"

NAMESPACE_BANG_BEGIN

class PhysicsObject
{
public:
    enum class Type
    {
        NONE,
        RIGIDBODY,
        BOX_COLLIDER,
        SPHERE_COLLIDER,
        CAPSULE_COLLIDER
    };

	PhysicsObject();
	virtual ~PhysicsObject();

    PhysicsObject::Type GetPhysicsObjectType() const;

protected:
    PhysicsObject::Type m_physicsObjectType = PhysicsObject::Type::NONE;

    void SetPhysicsObjectType(PhysicsObject::Type physicsObjectType);
};

NAMESPACE_BANG_END

#endif // PHYSICSOBJECT_H

