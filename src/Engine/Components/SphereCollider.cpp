#include "Bang/SphereCollider.h"

#include "Bang/Assert.h"
#include "Bang/AssetHandle.h"
#include "Bang/ClassDB.h"
#include "Bang/GameObject.h"
#include "Bang/MaterialFactory.h"
#include "Bang/Math.h"
#include "Bang/MetaNode.h"
#include "Bang/MetaNode.tcc"
#include "Bang/Physics.h"
#include "Bang/PhysicsComponent.h"
#include "Bang/PhysicsMaterial.h"
#include "Bang/Sphere.h"
#include "Bang/Transform.h"
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
    SET_INSTANCE_CLASS_ID(SphereCollider)
    SetPhysicsComponentType(PhysicsComponent::Type::SPHERE_COLLIDER);
    SetPhysicsMaterial(MaterialFactory::GetDefaultPhysicsMaterial().Get());
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

void SphereCollider::Reflect()
{
    Collider::Reflect();

    BANG_REFLECT_VAR_MEMBER_HINTED(SphereCollider,
                                   "Radius",
                                   SetRadius,
                                   GetRadius,
                                   BANG_REFLECT_HINT_MIN_VALUE(0.0001f));
}

float SphereCollider::GetRadius() const
{
    return m_radius;
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

    if (GetPxShape())
    {
        float scaledRadius = GetScaledRadius();
        physx::PxSphereGeometry sphereGeometry;
        sphereGeometry.radius = scaledRadius;
        GetPxShape()->setGeometry(sphereGeometry);

        if (GetPxRigidDynamic())
        {
            physx::PxRigidBodyExt::updateMassAndInertia(*GetPxRigidDynamic(),
                                                        1.0f);
        }
    }
}
