#ifndef PHYSICSCOMPONENT_H
#define PHYSICSCOMPONENT_H

#include "Bang/BangDefines.h"
#include "Bang/Component.h"

namespace physx
{
class PxActor;
class PxRigidActor;
class PxRigidStatic;
class PxRigidDynamic;
}

namespace Bang
{
class PhysicsComponent : public Component
{
    COMPONENT_ABSTRACT(PhysicsComponent)

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

    PhysicsComponent();
    virtual ~PhysicsComponent() override;

    // Component
    virtual void OnStart() override;
    virtual void OnUpdate() override;

    void SetStatic(bool isStatic);

    bool GetStatic() const;
    physx::PxRigidActor *GetPxRigidActor() const;
    physx::PxRigidStatic *GetPxRigidStatic() const;
    physx::PxRigidDynamic *GetPxRigidDynamic() const;
    PhysicsComponent::Type GetPhysicsComponentType() const;

protected:
    virtual void SetPxEnabled(bool pxEnabled) = 0;
    void SetPxRigidActor(physx::PxRigidActor *pxRigidActor);
    void SetPhysicsComponentType(PhysicsComponent::Type physicsObjectType);

    virtual void OnPxRigidActorChanged(physx::PxRigidActor *prevPxRigidActor,
                                       physx::PxRigidActor *newPxRigidActor);

private:
    bool m_static = false;
    bool m_previousEnabled = true;

    physx::PxRigidActor *p_pxRigidActor = nullptr;
    PhysicsComponent::Type m_physicsObjectType = PhysicsComponent::Type::NONE;

    friend class PxSceneContainer;
};
}

#endif  // PHYSICSCOMPONENT_H
