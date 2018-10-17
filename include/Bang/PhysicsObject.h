#ifndef PHYSICSOBJECT_H
#define PHYSICSOBJECT_H

#include "Bang/BangDefines.h"

FORWARD namespace physx
{
FORWARD class PxRigidDynamic;
}

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
        CAPSULE_COLLIDER,
        MESH_COLLIDER
    };

	PhysicsObject();
    virtual ~PhysicsObject();

    physx::PxRigidDynamic *GetPxRigidDynamic() const;
    PhysicsObject::Type GetPhysicsObjectType() const;

protected:
    physx::PxRigidDynamic *p_pxRigidDynamic = nullptr;
    PhysicsObject::Type m_physicsObjectType = PhysicsObject::Type::NONE;

    void SetPxRigidDynamic(physx::PxRigidDynamic *pxRigidDynamic);
    void SetPhysicsObjectType(PhysicsObject::Type physicsObjectType);

    virtual void OnPxRigidDynamicChanged(
                            physx::PxRigidDynamic *prevPxRigidDynamic,
                            physx::PxRigidDynamic *newPxRigidDynamic);

    friend class PxSceneContainer;
};

NAMESPACE_BANG_END

#endif // PHYSICSOBJECT_H

