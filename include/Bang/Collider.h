#ifndef COLLIDER_H
#define COLLIDER_H

#include "Bang/BangDefines.h"
#include "Bang/Component.h"
#include "Bang/ComponentMacros.h"
#include "Bang/MetaNode.h"
#include "Bang/PhysicsObject.h"
#include "Bang/ResourceHandle.h"
#include "Bang/String.h"
#include "Bang/Vector3.h"

namespace physx
{
class PxRigidDynamic;
class PxTransform;
class PxShape;
}

namespace Bang
{
class ICloneable;
class Object;
class PhysicsMaterial;

#define COLLIDER(className)                     \
    COMPONENT(className) \
    friend class Physics;                       \
    friend class PxSceneContainer;

class Collider : public Component, public PhysicsObject
{
    COMPONENT_ABSTRACT(Collider)

public:
    Collider();
    virtual ~Collider() override;

    // Component
    void OnUpdate() override;

    virtual void UpdatePxShape();
    void SetIsTrigger(bool isTrigger);
    void SetCenter(const Vector3 &center);
    void SetPhysicsMaterial(PhysicsMaterial *physicsMaterial);
    void SetUseInNavMesh(bool useInNavMesh);

    bool GetIsTrigger() const;
    bool GetUseInNavMesh() const;
    const Vector3 &GetCenter() const;
    PhysicsMaterial *GetSharedPhysicsMaterial() const;
    PhysicsMaterial *GetActivePhysicsMaterial() const;
    PhysicsMaterial *GetPhysicsMaterial() const;

    // Serializable
    virtual void Reflect() override;

protected:
    virtual physx::PxShape *CreatePxShape() const = 0;

    physx::PxTransform GetWorldPxTransform() const;
    physx::PxTransform GetWorldPxTransformWithRespectToPxActor() const;
    Matrix4 GetWorldShapeTransform() const;
    Matrix4 GetWorldShapeTransformWithRespectToPxActor() const;
    Matrix4 GetLocalShapeTransform() const;
    virtual Quaternion GetInternalRotation() const;

    physx::PxShape *GetPxShape() const;

    // Object
    void OnEnabled(Object *object) override;
    void OnDisabled(Object *object) override;

private:
    bool m_isTrigger = false;
    bool m_useInNavMesh = true;
    Vector3 m_center = Vector3::Zero();

    mutable RH<PhysicsMaterial> p_physicsMaterial;
    RH<PhysicsMaterial> p_sharedPhysicsMaterial;

    physx::PxShape *p_pxShape = nullptr;

    // PhysicsObject
    virtual bool CanComputeInertia() const;
    virtual bool CanBeSimulationShape() const;
    virtual bool CanBeTriggerShape() const;
    virtual void OnPxRigidActorChanged(
        physx::PxRigidActor *prevPxRigidActor,
        physx::PxRigidActor *newPxRigidActor) override;

    friend class Physics;
    friend class PxSceneContainer;
};
}

#endif  // COLLIDER_H
