#ifndef PHYSICSOBJECT_H
#define PHYSICSOBJECT_H

#include "Bang/BangDefines.h"

namespace physx
{
class PxActor;
class PxRigidActor;
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

    void SetStatic(bool isStatic);

    bool GetStatic() const;
    physx::PxRigidActor *GetPxRigidActor() const;
    physx::PxRigidStatic *GetPxRigidStatic() const;
    physx::PxRigidDynamic *GetPxRigidDynamic() const;
    PhysicsObject::Type GetPhysicsObjectType() const;

protected:
    void SetPxRigidActor(physx::PxRigidActor *pxRigidActor);
    void SetPhysicsObjectType(PhysicsObject::Type physicsObjectType);

    virtual void OnPxRigidActorChanged(physx::PxRigidActor *prevPxRigidActor,
                                       physx::PxRigidActor *newPxRigidActor);

private:
    bool m_static = false;
    physx::PxRigidActor *p_pxRigidActor = nullptr;
    PhysicsObject::Type m_physicsObjectType = PhysicsObject::Type::NONE;

    friend class PxSceneContainer;
};
}

#endif  // PHYSICSOBJECT_H
