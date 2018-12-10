#ifndef SPHERECOLLIDER_H
#define SPHERECOLLIDER_H

#include "Bang/BangDefines.h"
#include "Bang/Collider.h"
#include "Bang/MetaNode.h"
#include "Bang/Sphere.h"
#include "Bang/String.h"
#include "PxShape.h"

namespace physx
{
}

namespace Bang
{
class ICloneable;

class SphereCollider : public Collider
{
    COLLIDER(SphereCollider)

public:
    SphereCollider();
    virtual ~SphereCollider() override;

    void SetRadius(float radius);

    float GetRadius() const;
    float GetScaledRadius() const;
    Sphere GetSphereWorld() const;

    // Serializable
    void Reflect() override;

protected:
    float m_radius = 1.0f;

    // Collider
    physx::PxShape *CreatePxShape() const override;
    void UpdatePxShape() override;
};
}

#endif  // SPHERECOLLIDER_H
