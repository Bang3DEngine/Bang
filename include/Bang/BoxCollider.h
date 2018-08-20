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

    // ICloneable
    virtual void CloneInto(ICloneable *clone) const override;

    // Serializable
    virtual void ImportXML(const XMLNode &xmlInfo) override;
    virtual void ExportXML(XMLNode *xmlInfo) const override;

protected:
    void UpdateShapeGeometry() override;
};

NAMESPACE_BANG_END

#endif // BOXCOLLIDER_H

