#ifndef PHYSICSOBJECT_H
#define PHYSICSOBJECT_H

#include "Bang/Bang.h"

FORWARD namespace physx
{
    FORWARD class PxRigidDynamic;
};

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

    physx::PxRigidDynamic *GetPxRigidDynamic() const;
    PhysicsObject::Type GetPhysicsObjectType() const;

protected:
    physx::PxRigidDynamic *p_pxRigidDynamic = nullptr;
    PhysicsObject::Type m_physicsObjectType = PhysicsObject::Type::NONE;

    virtual void OnPxRigidDynamicSet();
    void SetPxRigidDynamic(physx::PxRigidDynamic *pxRigidDynamic);
    void SetPhysicsObjectType(PhysicsObject::Type physicsObjectType);

    friend class PxSceneContainer;
};

NAMESPACE_BANG_END

#endif // PHYSICSOBJECT_H

