#ifndef CAPSULECOLLIDER_H
#define CAPSULECOLLIDER_H

#include "Bang/Bang.h"
#include "Bang/Axis.h"
#include "Bang/Collider.h"

NAMESPACE_BANG_BEGIN

class CapsuleCollider : public Collider
{
    COLLIDER(CapsuleCollider)

public:
	CapsuleCollider();
	virtual ~CapsuleCollider();

    void SetRadius(float radius);
    void SetHeight(float height);
    void SetAxis(Axis3D axis);

    Axis3D GetAxis() const;
    float GetRadius() const;
    float GetHeight() const;
    float GetScaledHeight() const;
    float GetScaledRadius() const;

    // ICloneable
    virtual void CloneInto(ICloneable *clone) const override;

    // Serializable
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;

protected:
    float m_radius = 0.5f;
    float m_height = 1.0f;
    Axis3D m_axis = Axis3D::Y;

    // Collider
    void UpdatePxShape() override;
    Quaternion GetInternalRotation() const override;
};

NAMESPACE_BANG_END

#endif // CAPSULECOLLIDER_H

