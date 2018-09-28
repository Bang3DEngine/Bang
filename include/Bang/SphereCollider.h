#ifndef SPHERECOLLIDER_H
#define SPHERECOLLIDER_H

#include "Bang/Bang.h"
#include "Bang/Collider.h"

NAMESPACE_BANG_BEGIN

class SphereCollider : public Collider
{
    COLLIDER(SphereCollider)

public:
	SphereCollider();
	virtual ~SphereCollider();

    void SetRadius(float radius);

    float GetRadius() const;
    float GetScaledRadius() const;
    Sphere GetSphereWorld() const;

    // ICloneable
    virtual void CloneInto(ICloneable *clone) const override;

    // Serializable
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;

protected:
    float m_radius = 1.0f;

    // Collider
    void UpdatePxShape() override;

};

NAMESPACE_BANG_END

#endif // SPHERECOLLIDER_H

