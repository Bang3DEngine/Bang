#ifndef BOXCOLLIDER_H
#define BOXCOLLIDER_H

#include "Bang/Bang.h"
#include "Bang/Collider.h"

NAMESPACE_BANG_BEGIN

class BoxCollider : public Collider
{
    COLLIDER(BoxCollider)

public:
	BoxCollider();
    virtual ~BoxCollider();

    void SetHalfExtents(const Vector3 &halfExtents);

    const Vector3& GetHalfExtents() const;

    // ICloneable
    virtual void CloneInto(ICloneable *clone) const override;

    // Serializable
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;

protected:
    Vector3 m_halfExtents = Vector3(0.5f);

    // Collider
    void UpdatePxShape() override;
};

NAMESPACE_BANG_END

#endif // BOXCOLLIDER_H

