#ifndef MESHCOLLIDER_H
#define MESHCOLLIDER_H

#include "Bang/Bang.h"
#include "Bang/BangDefines.h"
#include "Bang/Collider.h"
#include "Bang/ComponentClassIds.h"
#include "Bang/MetaNode.h"
#include "Bang/ResourceHandle.h"
#include "Bang/String.h"

FORWARD namespace physx
{
FORWARD class PxShape;
}

NAMESPACE_BANG_BEGIN

FORWARD class ICloneable;
FORWARD class Mesh;

class MeshCollider : public Collider
{
    COLLIDER(MeshCollider)

public:
	MeshCollider();
	virtual ~MeshCollider() override;

    void SetMesh(Mesh *mesh);

    Mesh *GetMesh() const;

    // ICloneable
    virtual void CloneInto(ICloneable *clone) const override;

    // Serializable
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;

private:
    RH<Mesh> p_mesh;

    // Collider
    virtual bool CanBeTriggerShape() override;
    virtual bool CanBeSimulationShape() override;
    physx::PxShape* CreatePxShape() const override;
    void UpdatePxShape() override;
};

NAMESPACE_BANG_END

#endif // MESHCOLLIDER_H

