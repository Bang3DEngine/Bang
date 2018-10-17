#ifndef BOXCOLLIDER_H
#define BOXCOLLIDER_H

#include "Bang/BangDefines.h"
#include "Bang/Box.h"
#include "Bang/Collider.h"
#include "Bang/ComponentClassIds.h"
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

    // ICloneable
    virtual void CloneInto(ICloneable *clone) const override;

    // Serializable
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;

protected:
    Vector3 m_extents = Vector3(0.5f);

    // Collider
    physx::PxShape *CreatePxShape() const override;
    void UpdatePxShape() override;
};
}

#endif  // BOXCOLLIDER_H
