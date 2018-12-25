#ifndef MESHCOLLIDER_H
#define MESHCOLLIDER_H

#include "Bang/AssetHandle.h"
#include "Bang/BangDefines.h"
#include "Bang/Collider.h"
#include "Bang/MetaNode.h"
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

    // Serializable
    void Reflect() override;

private:
    AH<Mesh> p_mesh;

    // Collider
    virtual bool CanComputeInertia() const override;
    virtual bool CanBeTriggerShape() const override;
    virtual bool CanBeSimulationShape() const override;
    physx::PxShape *CreatePxShape() const override;
    void UpdatePxShape() override;
};
}

#endif  // MESHCOLLIDER_H
