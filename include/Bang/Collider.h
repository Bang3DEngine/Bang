#ifndef COLLIDER_H
#define COLLIDER_H

#include "Bang/Bang.h"
#include "Bang/Component.h"
#include "Bang/PhysicsObject.h"

FORWARD namespace physx
{
    FORWARD class PxShape;
    FORWARD class PxRigidBody;
}

NAMESPACE_BANG_BEGIN

#define COLLIDER(className) \
            COMPONENT(className) \
            friend class Physics;

class Collider : public PhysicsObject,
                 public Component
{
public:
	Collider();
    virtual ~Collider();

    // Component
    void OnUpdate() override;

    void SetCenter(const Vector3 &center);

    const Vector3& GetCenter() const;

    // ICloneable
    virtual void CloneInto(ICloneable *clone) const override;

    // Serializable
    virtual void ImportXML(const XMLNode &xmlInfo) override;
    virtual void ExportXML(XMLNode *xmlInfo) const override;

protected:
    virtual void UpdateShapeGeometry();

    void SetPxRigidBody(physx::PxRigidBody *pxRB);
    void SetPxShape(physx::PxShape *pxShape);

    physx::PxRigidBody* GetPxRigidBody() const;
    physx::PxShape* GetPxShape() const;

private:
    Vector3 m_center = Vector3::Zero;

    physx::PxShape *p_pxShape = nullptr;
    physx::PxRigidBody *p_pxRigidBody = nullptr;

    friend class Physics;
};

NAMESPACE_BANG_END

#endif // COLLIDER_H

