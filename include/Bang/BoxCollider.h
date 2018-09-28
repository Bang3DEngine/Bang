#ifndef BOXCOLLIDER_H
#define BOXCOLLIDER_H

#include "Bang/Box.h"
#include "Bang/Collider.h"

NAMESPACE_BANG_BEGIN

class BoxCollider : public Collider
{
    COLLIDER(BoxCollider)

public:
	BoxCollider();
    virtual ~BoxCollider();

    void SetExtents(const Vector3 &extents);

    Box GetBoxWorld() const;
    const Vector3& GetExtents() const;

    // ICloneable
    virtual void CloneInto(ICloneable *clone) const override;

    // Serializable
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;

protected:
    Vector3 m_extents = Vector3(0.5f);

    // Collider
    void UpdatePxShape() override;
};

NAMESPACE_BANG_END

#endif // BOXCOLLIDER_H

