#ifndef COLLIDER_H
#define COLLIDER_H

#include "Bang/Bang.h"
#include "Bang/Component.h"
#include "Bang/PhysicsObject.h"

NAMESPACE_BANG_BEGIN

class Collider : public PhysicsObject,
                 public Component
{
    COMPONENT(Collider)

public:
	Collider();
	virtual ~Collider();

    // ICloneable
    virtual void CloneInto(ICloneable *clone) const override;

    // Serializable
    virtual void ImportXML(const XMLNode &xmlInfo) override;
    virtual void ExportXML(XMLNode *xmlInfo) const override;
};

NAMESPACE_BANG_END

#endif // COLLIDER_H

