#include "Bang/SphereCollider.h"

#include "Bang/Assert.h"
#include "Bang/FastDynamicCast.h"
#include "Bang/GameObject.h"
#include "Bang/MaterialFactory.h"
#include "Bang/Math.h"
#include "Bang/MetaNode.h"
#include "Bang/MetaNode.tcc"
#include "Bang/Physics.h"
#include "Bang/PhysicsMaterial.h"
#include "Bang/PhysicsObject.h"
#include "Bang/ResourceHandle.h"
#include "Bang/Sphere.h"
#include "Bang/Transform.h"
#include "Bang/Vector.tcc"
#include "Bang/Vector3.h"
#include "PxRigidDynamic.h"
#include "PxShape.h"
#include "extensions/PxRigidBodyExt.h"
#include "geometry/PxSphereGeometry.h"

namespace Bang
{
class ICloneable;
}

using namespace Bang;

SphereCollider::SphereCollider()
{
    CONSTRUCT_CLASS_ID(SphereCollider)
    SetPhysicsObjectType(PhysicsObject::Type::SPHERE_COLLIDER);
    SetPhysicsMaterial(MaterialFactory::GetDefaultPhysicsMaterial().Get());
}

SphereCollider::~SphereCollider()
{
}

void SphereCollider::SetRadius(float radius)
{
    if(radius != GetRadius())
    {
        m_radius = radius;
        UpdatePxShape();
    }
}

float SphereCollider::GetScaledRadius() const
{
    Vector3 scale = GetGameObject()->GetTransform()->GetScale();
    float maxScale = Math::Max(Math::Max(scale.x, scale.y), scale.z);
    return maxScale * GetRadius();
}

Sphere SphereCollider::GetSphereWorld() const
{
    Transform *tr = GetGameObject()->GetTransform();

    Sphere sphere;
    sphere.SetRadius(GetScaledRadius());
    sphere.SetCenter(tr->GetPosition() + GetCenter());
    return sphere;
}

float SphereCollider::GetRadius() const
{
    return m_radius;
}

void SphereCollider::CloneInto(ICloneable *clone) const
{
    Collider::CloneInto(clone);

    SphereCollider *scClone = SCAST<SphereCollider *>(clone);
    scClone->SetRadius(GetRadius());
}

void SphereCollider::ImportMeta(const MetaNode &metaNode)
{
    Collider::ImportMeta(metaNode);

    if(metaNode.Contains("Radius"))
    {
        SetRadius(metaNode.Get<float>("Radius"));
    }
}

void SphereCollider::ExportMeta(MetaNode *metaNode) const
{
    Collider::ExportMeta(metaNode);

    metaNode->Set("Radius", GetRadius());
}

physx::PxShape *SphereCollider::CreatePxShape() const
{
    return GetPxRigidDynamic()
               ? GetPxRigidDynamic()->createShape(
                     physx::PxSphereGeometry(1),
                     *Physics::GetDefaultPxMaterial())
               : nullptr;
}

void SphereCollider::UpdatePxShape()
{
    Collider::UpdatePxShape();

    if(GetPxShape())
    {
        ASSERT(GetPxRigidDynamic());

        float scaledRadius = GetScaledRadius();
        physx::PxSphereGeometry sphereGeometry;
        sphereGeometry.radius = scaledRadius;
        GetPxShape()->setGeometry(sphereGeometry);

        physx::PxRigidBodyExt::updateMassAndInertia(*GetPxRigidDynamic(), 1.0f);
    }
}
