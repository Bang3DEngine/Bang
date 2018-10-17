#ifndef MESHCOLLIDER_H
#define MESHCOLLIDER_H

#include "Bang/BangDefines.h"
#include "Bang/Collider.h"
#include "Bang/ComponentClassIds.h"
#include "Bang/MetaNode.h"
#include "Bang/ResourceHandle.h"
#include "Bang/String.h"
#include "PxShape.h"

namespace physx
{
}

namespace Bang
{
class ICloneable;
class Mesh;

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
    physx::PxShape *CreatePxShape() const override;
    void UpdatePxShape() override;
};
}

#endif  // MESHCOLLIDER_H
