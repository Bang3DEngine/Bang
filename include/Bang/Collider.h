#ifndef COLLIDER_H
#define COLLIDER_H

#include "Bang/Bang.h"
#include "Bang/Component.h"
#include "Bang/PhysicsObject.h"
#include "Bang/PhysicsMaterial.h"

FORWARD namespace physx
{
    FORWARD class PxShape;
    FORWARD class PxRigidBody;
}

NAMESPACE_BANG_BEGIN

#define COLLIDER(className) \
        COMPONENT_WITH_FAST_DYNAMIC_CAST(className) \
        friend class Physics; \
        friend class PxSceneContainer;

class Collider : public Component,
                 public PhysicsObject
{
    COMPONENT_WITH_FAST_DYNAMIC_CAST_ABSTRACT(Collider)

public:
	Collider();
    virtual ~Collider();

    // Component
    void OnUpdate() override;

    void SetIsTrigger(bool isTrigger);
    void SetCenter(const Vector3 &center);
    void SetPhysicsMaterial(PhysicsMaterial *physicsMaterial);

    bool GetIsTrigger() const;
    const Vector3& GetCenter() const;
    PhysicsMaterial* GetSharedPhysicsMaterial() const;
    PhysicsMaterial* GetActivePhysicsMaterial() const;
    PhysicsMaterial *GetPhysicsMaterial() const;

    // ICloneable
    virtual void CloneInto(ICloneable *clone) const override;

    // Serializable
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;

protected:
    virtual physx::PxShape* CreatePxShape() const = 0;

    Matrix4 GetShapeTransformWithRespectToPxActor() const;
    virtual Quaternion GetInternalRotation() const;
    virtual void UpdatePxShape();

    physx::PxShape* GetPxShape() const;

    // Object
    void OnEnabled(Object *object) override;
    void OnDisabled(Object *object) override;

private:
    bool m_isTrigger = false;
    Vector3 m_center = Vector3::Zero;

    mutable RH<PhysicsMaterial> p_physicsMaterial;
    RH<PhysicsMaterial> p_sharedPhysicsMaterial;

    physx::PxShape *p_pxShape = nullptr;

    // PhysicsObject
    virtual bool CanBeSimulationShape();
    virtual bool CanBeTriggerShape();
    virtual void OnPxRigidDynamicChanged(physx::PxRigidDynamic *prevPxRD,
                                         physx::PxRigidDynamic *newPxRD) override;

    friend class Physics;
    friend class PxSceneContainer;
};

NAMESPACE_BANG_END

#endif // COLLIDER_H

