#ifndef BOXCOLLIDER_H
#define BOXCOLLIDER_H

#include "Bang/BangDefines.h"
#include "Bang/Box.h"
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

class BoxCollider : public Collider
{
    COLLIDER(BoxCollider)

public:
    BoxCollider();
    virtual ~BoxCollider() override;

    void SetExtents(const Vector3 &extents);

    Box GetBoxWorld() const;
    const Vector3 &GetExtents() const;

    // Serializable
    void Reflect() override;

protected:
    Vector3 m_extents = Vector3(0.5f);

    // Collider
    physx::PxShape *CreatePxShape() const override;
    void UpdatePxShape() override;
};
}

#endif  // BOXCOLLIDER_H
