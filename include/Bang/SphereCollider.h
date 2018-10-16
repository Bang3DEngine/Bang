#ifndef SPHERECOLLIDER_H
#define SPHERECOLLIDER_H

#include "Bang/Bang.h"
#include "Bang/BangDefines.h"
#include "Bang/Collider.h"
#include "Bang/ComponentClassIds.h"
#include "Bang/MetaNode.h"
#include "Bang/Sphere.h"
#include "Bang/String.h"

FORWARD namespace physx
{
FORWARD class PxShape;
}

NAMESPACE_BANG_BEGIN

FORWARD class ICloneable;

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

    // ICloneable
    virtual void CloneInto(ICloneable *clone) const override;

    // Serializable
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;

protected:
    float m_radius = 1.0f;

    // Collider
    physx::PxShape* CreatePxShape() const override;
    void UpdatePxShape() override;

};

NAMESPACE_BANG_END

#endif // SPHERECOLLIDER_H

