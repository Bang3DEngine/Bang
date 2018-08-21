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

    void SetRadius(const float radius);

    float GetRadius() const;
    float GetScaledRadius() const;

    // ICloneable
    virtual void CloneInto(ICloneable *clone) const override;

    // Serializable
    virtual void ImportXML(const XMLNode &xmlInfo) override;
    virtual void ExportXML(XMLNode *xmlInfo) const override;

protected:
    float m_radius = 1.0f;

    void UpdateShapeGeometry() override;

};

NAMESPACE_BANG_END

#endif // SPHERECOLLIDER_H

