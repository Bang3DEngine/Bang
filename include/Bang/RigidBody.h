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

NAMESPACE_BANG_BEGIN

class RigidBody : public PhysicsObject,
                  public Component
{
    COMPONENT(RigidBody)

public:
	RigidBody();
    virtual ~RigidBody();

    void SetMass(float mass);
    void SetDrag(float drag);
    void SetAngularDrag(float angularDrag);
    void SetUseGravity(bool useGravity);
    void SetIsKinematic(bool isKinematic);
    void SetConstraints(const RigidBodyConstraints &constraints);

    float GetMass() const;
    float GetDrag() const;
    float GetAngularDrag() const;
    bool GetUseGravity() const;
    bool GetIsKinematic() const;
    const RigidBodyConstraints& GetConstraints() const;

    // ICloneable
    virtual void CloneInto(ICloneable *clone) const override;

    // Serializable
    virtual void ImportXML(const XMLNode &xmlInfo) override;
    virtual void ExportXML(XMLNode *xmlInfo) const override;

private:
    float m_mass = 1.0f;
    float m_drag = 0.0f;
    float m_angularDrag = 0.05f;
    bool m_useGravity = true;
    bool m_isKinematic = false;
    RigidBodyConstraints m_constraints = RigidBodyConstraint::NONE;

    physx::PxRigidDynamic *p_pxRigidDynamic = nullptr;

    void UpdatePxRigidDynamicValues();
    void SetPxRigidDynamic(physx::PxRigidDynamic *pxRigidDynamic);

    physx::PxRigidDynamic* GetPxRigidDynamic() const;

    friend class Physics;
    friend class PxSceneContainer;
};

NAMESPACE_BANG_END

#endif // RIGIDBODY_H

