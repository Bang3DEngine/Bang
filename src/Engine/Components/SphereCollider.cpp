#include "Bang/SphereCollider.h"

#include "Bang/Physics.h"
#include "Bang/XMLNode.h"
#include "Bang/Transform.h"
#include "Bang/GameObject.h"

USING_NAMESPACE_BANG

SphereCollider::SphereCollider()
{
    SetPhysicsObjectType( PhysicsObject::Type::SPHERE_COLLIDER );
}

SphereCollider::~SphereCollider()
{
}

void SphereCollider::SetRadius(const float radius)
{
    if (radius != GetRadius())
    {
        m_radius = radius;
        UpdateShapeGeometry();
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

void SphereCollider::ImportXML(const XMLNode &xmlInfo)
{
    Collider::ImportXML(xmlInfo);

    if (xmlInfo.Contains("Radius"))
    {
        SetRadius( xmlInfo.Get<float>("Radius") );
    }
}

void SphereCollider::ExportXML(XMLNode *xmlInfo) const
{
    Collider::ExportXML(xmlInfo);

    xmlInfo->Set("Radius", GetRadius());
}

void SphereCollider::UpdateShapeGeometry()
{
    Collider::UpdateShapeGeometry();

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
