#ifndef RIGIDBODY_H
#define RIGIDBODY_H

#include "Bang/Bang.h"
#include "Bang/Physics.h"
#include "Bang/Component.h"
#include "Bang/PhysicsObject.h"

FORWARD namespace physx
{
    FORWARD class PxRigidDynamic;
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
    FORCE           = physx::PxForceMode::eFORCE,
    IMPULSE         = physx::PxForceMode::eIMPULSE,
    ACCELERATION    = physx::PxForceMode::eACCELERATION,
    VELOCITY_CHANGE = physx::PxForceMode::eVELOCITY_CHANGE
};

NAMESPACE_BANG_BEGIN

class RigidBody : public Component,
                  public PhysicsObject
{
    COMPONENT_WITH_FAST_DYNAMIC_CAST(RigidBody)

public:
	RigidBody();
    virtual ~RigidBody();

    void AddForce(const Vector3 &force, ForceMode forceMode = ForceMode::FORCE);
    void AddTorque(const Vector3 &torque, ForceMode forceMode = ForceMode::FORCE);
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
    const RigidBodyConstraints& GetConstraints() const;

    // ICloneable
    virtual void CloneInto(ICloneable *clone) const override;

    // Serializable
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;

private:
    physx::PxRigidDynamic *p_pxRigidDynamic = nullptr;

    // Saved properties
    float m_mass = 1.0f;
    float m_drag = 0.0f;
    float m_angularDrag = 0.05f;
    bool m_useGravity = true;
    bool m_isKinematic = false;
    RigidBodyConstraints m_constraints = RigidBodyConstraint::NONE;

    // Properties only saved to init the rigidbody when registered
    bool m_initRigidDynamic = true;
    Vector3 m_initLinearVelocity = Vector3::Zero;
    Vector3 m_initAngularVelocity = Vector3::Zero;
    float m_initMaxAngularVelocity = 0.0f;

    void UpdatePxRigidDynamicValues();
    void SetPxRigidDynamic(physx::PxRigidDynamic *pxRigidDynamic);

    physx::PxRigidDynamic* GetPxRigidDynamic() const;

    friend class Physics;
    friend class PxSceneContainer;
};

NAMESPACE_BANG_END

#endif // RIGIDBODY_H

