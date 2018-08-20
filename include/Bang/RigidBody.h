#ifndef RIGIDBODY_H
#define RIGIDBODY_H

#include "Bang/Bang.h"
#include "Bang/Component.h"
#include "Bang/PhysicsObject.h"

NAMESPACE_BANG_BEGIN

class RigidBody : public PhysicsObject,
                  public Component
{
    COMPONENT(RigidBody)

public:
	RigidBody();
    virtual ~RigidBody();

    // ICloneable
    virtual void CloneInto(ICloneable *clone) const override;

    // Serializable
    virtual void ImportXML(const XMLNode &xmlInfo) override;
    virtual void ExportXML(XMLNode *xmlInfo) const override;
};

NAMESPACE_BANG_END

#endif // RIGIDBODY_H

