#include "Bang/MeshCollider.h"

#include "Bang/FastDynamicCast.h"
#include "Bang/GUID.h"
#include "Bang/Material.h"
#include "Bang/MaterialFactory.h"
#include "Bang/Mesh.h"
#include "Bang/MetaNode.h"
#include "Bang/MetaNode.tcc"
#include "Bang/Physics.h"
#include "Bang/PhysicsMaterial.h"
#include "Bang/PhysicsObject.h"
#include "Bang/Resources.h"
#include "Bang/Resources.tcc"
#include "PxRigidDynamic.h"
#include "PxShape.h"
#include "extensions/PxRigidActorExt.h"
#include "foundation/Px.h"
#include "foundation/PxQuat.h"
#include "foundation/PxVec3.h"
#include "geometry/PxMeshScale.h"
#include "geometry/PxTriangleMeshGeometry.h"

namespace Bang
{
class ICloneable;
}

namespace physx
{
class PxTriangleMesh;
}

using namespace Bang;
using namespace physx;

MeshCollider::MeshCollider()
{
    CONSTRUCT_CLASS_ID(Collider)
    SetPhysicsObjectType(PhysicsObject::Type::MESH_COLLIDER);
    SetPhysicsMaterial(MaterialFactory::GetDefaultPhysicsMaterial().Get());
}

MeshCollider::~MeshCollider()
{
}

void MeshCollider::SetMesh(Mesh *mesh)
{
    if(mesh != GetMesh())
    {
        p_mesh.Set(mesh);
        UpdatePxShape();
    }
}

Mesh *MeshCollider::GetMesh() const
{
    return p_mesh.Get();
}

void MeshCollider::CloneInto(ICloneable *clone) const
{
    Collider::CloneInto(clone);

    MeshCollider *mcClone = SCAST<MeshCollider *>(clone);
    mcClone->SetMesh(GetMesh());
}

void MeshCollider::ImportMeta(const MetaNode &metaNode)
{
    Collider::ImportMeta(metaNode);

    if(metaNode.Contains("Mesh"))
    {
        SetMesh(Resources::Load<Mesh>(metaNode.Get<GUID>("Mesh")).Get());
    }
}

void MeshCollider::ExportMeta(MetaNode *metaNode) const
{
    Collider::ExportMeta(metaNode);
    metaNode->Set("Mesh", GetMesh() ? GetMesh()->GetGUID() : GUID::Empty());
}

bool MeshCollider::CanBeTriggerShape()
{
    return false;
}

bool MeshCollider::CanBeSimulationShape()
{
    return false;
}

physx::PxShape *MeshCollider::CreatePxShape() const
{
    if(PxRigidDynamic *pxRD = GetPxRigidDynamic())
    {
        PxMeshScale scale(PxVec3(1, 1, 1), PxQuat(PxIdentity));
        PxTriangleMesh *pxTriMesh =
            Physics::GetInstance()->CreatePxTriangleMesh(GetMesh());
        PxTriangleMeshGeometry triMeshGeom(pxTriMesh, scale);
        PxShape *shape = nullptr;
        if(triMeshGeom.isValid())
        {
            shape = PxRigidActorExt::createExclusiveShape(
                *pxRD, triMeshGeom, *Physics::GetDefaultPxMaterial(),
                physx::PxShapeFlag::eSCENE_QUERY_SHAPE);
        }
        return shape;
    }
    else
    {
        return nullptr;
    }
}

void MeshCollider::UpdatePxShape()
{
    Collider::UpdatePxShape();

    if(GetPxShape())
    {
    }
}
