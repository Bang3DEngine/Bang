#include "Bang/MeshCollider.h"

#include "Bang/Mesh.h"
#include "Bang/Physics.h"
#include "Bang/MetaNode.h"
#include "Bang/Resources.h"
#include "Bang/MaterialFactory.h"

using namespace physx;

USING_NAMESPACE_BANG


MeshCollider::MeshCollider()
{
    CONSTRUCT_CLASS_ID(Collider)
    SetPhysicsObjectType( PhysicsObject::Type::MESH_COLLIDER );
    SetPhysicsMaterial( MaterialFactory::GetDefaultPhysicsMaterial().Get() );
}

MeshCollider::~MeshCollider()
{
}

void MeshCollider::SetMesh(Mesh *mesh)
{
    if (mesh != GetMesh())
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

    MeshCollider *mcClone = SCAST<MeshCollider*>(clone);
    mcClone->SetMesh( GetMesh() );
}

void MeshCollider::ImportMeta(const MetaNode &metaNode)
{
    Collider::ImportMeta(metaNode);

    if (metaNode.Contains("Mesh"))
    {
        SetMesh( Resources::Load<Mesh>( metaNode.Get<GUID>("Mesh") ).Get() );
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
    if (PxRigidDynamic *pxRD = GetPxRigidDynamic())
    {
        PxMeshScale scale(PxVec3(1,1,1), PxQuat(PxIdentity));
        PxTriangleMesh *pxTriMesh =
                    Physics::GetInstance()->CreatePxTriangleMesh( GetMesh() );
        PxTriangleMeshGeometry triMeshGeom(pxTriMesh, scale);
        PxShape *shape = PxRigidActorExt::createExclusiveShape(
                                          *pxRD,
                                          triMeshGeom,
                                          *Physics::GetDefaultPxMaterial(),
                                          physx::PxShapeFlag::eTRIGGER_SHAPE);
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

    if (GetPxShape())
    {
    }
}

