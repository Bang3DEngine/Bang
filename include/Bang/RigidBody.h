#ifndef RIGIDBODY_H
#define RIGIDBODY_H

#include "Bang/BangDefines.h"
#include "Bang/Component.h"
#include "Bang/ComponentMacros.h"
#include "Bang/Flags.h"
#include "Bang/MetaNode.h"
#include "Bang/PhysicsComponent.h"
#include "Bang/String.h"
#include "PxForceMode.h"
#include "PxRigidDynamic.h"

namespace physx
{
}

enum class RigidBodyConstraint
{
    NONE,
    LOCK_POSITION_X = physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_X,
    LOCK_POSITION_Y = physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Y,
    LOCK_POSITION_Z = physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Z,
    LOCK_ROTATION_X = physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X,
    LOCK_ROTATION_Y = physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y,
    LOCK_ROTATION_Z = physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z,
    DEFAULT = NONE
};
CREATE_FLAGS(RigidBodyConstraints, RigidBodyConstraint);

enum class ForceMode
{
    FORCE = physx::PxForceMode::eFORCE,
    IMPULSE = physx::PxForceMode::eIMPULSE,
    ACCELERATION = physx::PxForceMode::eACCELERATION,
    VELOCITY_CHANGE = physx::PxForceMode::eVELOCITY_CHANGE
};

namespace Bang
{
class ICloneable;

class RigidBody : public PhysicsComponent
{
    COMPONENT(RigidBody)

public:
    RigidBody();
    virtual ~RigidBody() override;

    void AddForce(const Vector3 &force, ForceMode forceMode = ForceMode::FORCE);
    void AddTorque(const Vector3 &torque,
                   ForceMode forceMode = ForceMode::FORCE);
    void AddForceAtPos(const Vector3 &force,
                       const Vector3 &pos,
                       ForceMode forceMode = ForceMode::FORCE);
    void AddForceAtLocalPos(const Vector3 &force,
                            const Vector3 &pos,
                            ForceMode forceMode = ForceMode::FORCE);
    void AddLocalForceAtPos(const Vector3 &force,
                            const Vector3 &pos,
                            ForceMode forceMode = ForceMode::FORCE);
    void AddLocalForceAtLocalPos(const Vector3 &force,
                                 const Vector3 &pos,
                                 ForceMode forceMode = ForceMode::FORCE);

    void ClearForce(ForceMode forceMode = ForceMode::FORCE);
    void ClearTorque(ForceMode forceMode = ForceMode::FORCE);

    void SetMass(float mass);
    void SetDrag(float drag);
    void SetAngularDrag(float angularDrag);
    void SetUseGravity(bool useGravity);
    void SetIsKinematic(bool isKinematic);
    void SetLinearVelocity(const Vector3 &linearVelocity);
    void SetAngularVelocity(const Vector3 &angularVelocity);
    void SetMaxAngularVelocity(float maxAngularVelocity);
    void SetConstraints(const RigidBodyConstraints &constraints);

    float GetMass() const;
    float GetDrag() const;
    float GetAngularDrag() const;
    bool GetUseGravity() const;
    bool GetIsKinematic() const;
    Vector3 GetLinearVelocity() const;
    Vector3 GetAngularVelocity() const;
    float GetMaxAngularVelocity() const;
    const RigidBodyConstraints &GetConstraints() const;

    // IReflectable
    virtual void Reflect() override;

private:
    // Saved properties
    float m_mass = 1.0f;
    float m_drag = 0.0f;
    float m_angularDrag = 0.05f;
    bool m_useGravity = true;
    bool m_isKinematic = true;
    RigidBodyConstraints m_constraints = RigidBodyConstraint::NONE;

    void UpdatePxRigidActorValues();

    // PhysicsComponent
    void SetPxEnabled(bool pxEnabled) override;
    void OnPxRigidActorChanged(physx::PxRigidActor *prevPxRigidActor,
                               physx::PxRigidActor *newPxRigidActor) override;

    friend class Physics;
    friend class PxSceneContainer;
};
}

#endif  // RIGIDBODY_H
