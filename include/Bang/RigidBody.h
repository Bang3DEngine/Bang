#ifndef RIGIDBODY_H
#define RIGIDBODY_H

#include "Bang/Bang.h"
#include "Bang/Component.h"
#include "Bang/PhysicsObject.h"

FORWARD namespace physx
{
    FORWARD class PxRigidDynamic;
}

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

    float GetMass() const;
    float GetDrag() const;
    float GetAngularDrag() const;
    bool GetUseGravity() const;
    bool GetIsKinematic() const;

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

    physx::PxRigidDynamic *p_pxRigidDynamic = nullptr;

    void UpdatePxRigidDynamicValues();
    void SetPxRigidDynamic(physx::PxRigidDynamic *pxRigidDynamic);

    physx::PxRigidDynamic* GetPxRigidDynamic() const;

    friend class Physics;
};

NAMESPACE_BANG_END

#endif // RIGIDBODY_H

