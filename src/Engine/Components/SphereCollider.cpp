#include "Bang/SphereCollider.h"

#include "Bang/Physics.h"
#include "Bang/MetaNode.h"
#include "Bang/Transform.h"
#include "Bang/GameObject.h"

USING_NAMESPACE_BANG

SphereCollider::SphereCollider()
{
    CONSTRUCT_CLASS_ID(SphereCollider)
    SetPhysicsObjectType( PhysicsObject::Type::SPHERE_COLLIDER );
}

SphereCollider::~SphereCollider()
{
}

void SphereCollider::SetRadius(float radius)
{
    if (radius != GetRadius())
    {
        m_radius = radius;
        UpdatePxShape();
    }
}

float SphereCollider::GetScaledRadius() const
{
    Vector3 scale = GetGameObject()->GetTransform()->GetScale();
    float maxScale = Math::Max( Math::Max(scale.x, scale.y), scale.z );
    return maxScale * GetRadius();
}

float SphereCollider::GetRadius() const
{
    return m_radius;
}

void SphereCollider::CloneInto(ICloneable *clone) const
{
    Collider::CloneInto(clone);

    SphereCollider *scClone = SCAST<SphereCollider*>(clone);
    scClone->SetRadius( GetRadius() );
}

void SphereCollider::ImportMeta(const MetaNode &metaNode)
{
    Collider::ImportMeta(metaNode);

    if (metaNode.Contains("Radius"))
    {
        SetRadius( metaNode.Get<float>("Radius") );
    }
}

void SphereCollider::ExportMeta(MetaNode *metaNode) const
{
    Collider::ExportMeta(metaNode);

    metaNode->Set("Radius", GetRadius());
}

void SphereCollider::UpdatePxShape()
{
    Collider::UpdatePxShape();

    if (GetPxShape())
    {
        ASSERT(GetPxRigidBody());
        ASSERT(GetPxShape());

        float scaledRadius = GetScaledRadius();
        physx::PxSphereGeometry sphereGeometry;
        sphereGeometry.radius = scaledRadius;
        GetPxShape()->setGeometry(sphereGeometry);

        physx::PxRigidBodyExt::updateMassAndInertia(*GetPxRigidBody(), 1.0f);
    }
}
