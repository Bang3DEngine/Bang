#include "Bang/MeshCollider.h"

#include "Bang/Assets.h"
#include "Bang/Assets.tcc"
#include "Bang/ClassDB.h"
#include "Bang/Extensions.h"
#include "Bang/GUID.h"
#include "Bang/GameObject.h"
#include "Bang/Material.h"
#include "Bang/MaterialFactory.h"
#include "Bang/Mesh.h"
#include "Bang/MetaNode.h"
#include "Bang/MetaNode.tcc"
#include "Bang/Physics.h"
#include "Bang/PhysicsComponent.h"
#include "Bang/PhysicsMaterial.h"
#include "Bang/Transform.h"
#include "PxRigidStatic.h"
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
    SET_INSTANCE_CLASS_ID(MeshCollider)
    SetPhysicsComponentType(PhysicsComponent::Type::MESH_COLLIDER);
    SetPhysicsMaterial(MaterialFactory::GetDefaultPhysicsMaterial().Get());
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

void MeshCollider::Reflect()
{
    BANG_REFLECT_VAR_ASSET(
        "Mesh",
        SetMesh,
        GetMesh,
        Mesh,
        BANG_REFLECT_HINT_EXTENSIONS(Extensions::GetMeshExtension()) +
            BANG_REFLECT_HINT_ZOOMABLE_PREVIEW(true));
}

bool MeshCollider::CanComputeInertia() const
{
    return false;
}

bool MeshCollider::CanBeTriggerShape() const
{
    return false;
}

bool MeshCollider::CanBeSimulationShape() const
{
    return true;
}

physx::PxShape *MeshCollider::CreatePxShape() const
{
    PxShape *shape = nullptr;
    if (PxRigidActor *pxRA = GetPxRigidActor())
    {
        PxMeshScale scale(PxVec3(1, 1, 1), PxQuat(PxIdentity));
        PxTriangleMesh *pxTriMesh =
            Physics::GetInstance()->CreatePxTriangleMesh(GetMesh());
        PxTriangleMeshGeometry triMeshGeom(pxTriMesh, scale);
        if (triMeshGeom.isValid())
        {
            shape = PxRigidActorExt::createExclusiveShape(
                *pxRA,
                triMeshGeom,
                *Physics::GetDefaultPxMaterial(),
                physx::PxShapeFlag::eSIMULATION_SHAPE);
        }
    }
    return shape;
}

void MeshCollider::UpdatePxShape()
{
    Collider::UpdatePxShape();

    if (GetPxShape())
    {
        Transform *tr = GetGameObject()->GetTransform();
        PxTriangleMeshGeometry &triMeshGeom =
            GetPxShape()->getGeometry().triangleMesh();

        PxVec3 scale = Physics::GetPxVec3FromVector3(tr->GetScale());
        if (triMeshGeom.scale.scale != scale)
        {
            triMeshGeom.scale.scale = scale;
            if (GetPxShape()->getGeometry().getType() ==
                PxGeometryType::eTRIANGLEMESH)
            {
                GetPxShape()->setGeometry(triMeshGeom);
            }
        }
    }
}
