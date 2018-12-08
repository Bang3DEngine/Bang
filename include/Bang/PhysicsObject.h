#ifndef PHYSICSOBJECT_H
#define PHYSICSOBJECT_H

#include "Bang/BangDefines.h"

namespace physx
{
class PxActor;
class PxRigidStatic;
class PxRigidDynamic;
}

namespace Bang
{
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

    physx::PxActor *GetPxActor() const;
    physx::PxRigidStatic *GetPxRigidStatic() const;
    physx::PxRigidDynamic *GetPxRigidDynamic() const;
    PhysicsObject::Type GetPhysicsObjectType() const;

protected:
    physx::PxActor *p_pxActor = nullptr;
    PhysicsObject::Type m_physicsObjectType = PhysicsObject::Type::NONE;

    void SetPxActor(physx::PxActor *pxActor);
    void SetPhysicsObjectType(PhysicsObject::Type physicsObjectType);

    virtual void OnPxActorChanged(physx::PxActor *prevPxActor,
                                  physx::PxActor *newPxActor);

    friend class PxSceneContainer;
};
}

#endif  // PHYSICSOBJECT_H
