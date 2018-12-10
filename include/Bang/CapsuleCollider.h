#ifndef CAPSULECOLLIDER_H
#define CAPSULECOLLIDER_H

#include "Bang/Axis.h"
#include "Bang/BangDefines.h"
#include "Bang/Collider.h"
#include "Bang/MetaNode.h"
#include "Bang/String.h"
#include "PxShape.h"

namespace physx
{
}

namespace Bang
{
class ICloneable;

class CapsuleCollider : public Collider
{
    COLLIDER(CapsuleCollider)

public:
    CapsuleCollider();
    virtual ~CapsuleCollider() override;

    void SetRadius(float radius);
    void SetHeight(float height);
    void SetAxis(Axis3D axis);

    Axis3D GetAxis() const;
    float GetRadius() const;
    float GetHeight() const;
    float GetScaledHeight() const;
    float GetScaledRadius() const;

    // IReflectable
    virtual void Reflect() override;

protected:
    float m_radius = 0.5f;
    float m_height = 1.0f;
    Axis3D m_axis = Axis3D::Y;

    // Collider
    physx::PxShape *CreatePxShape() const override;
    void UpdatePxShape() override;
    Quaternion GetInternalRotation() const override;
};
}

#endif  // CAPSULECOLLIDER_H
