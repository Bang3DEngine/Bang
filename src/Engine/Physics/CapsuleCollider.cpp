#include "Bang/CapsuleCollider.h"

#include <istream>

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
#include "Bang/Transform.h"
#include "PxRigidDynamic.h"
#include "PxShape.h"
#include "extensions/PxRigidBodyExt.h"
#include "geometry/PxCapsuleGeometry.h"

namespace Bang
{
class ICloneable;
}

using namespace Bang;

CapsuleCollider::CapsuleCollider()
{
    SET_INSTANCE_CLASS_ID(CapsuleCollider)
    SetPhysicsComponentType(PhysicsComponent::Type::CAPSULE_COLLIDER);
    SetPhysicsMaterial(MaterialFactory::GetDefaultPhysicsMaterial().Get());
}

CapsuleCollider::~CapsuleCollider()
{
}

void CapsuleCollider::SetRadius(float radius)
{
    if (radius != GetRadius())
    {
        m_radius = radius;
        UpdatePxShape();
    }
}

void CapsuleCollider::SetHeight(float height)
{
    if (height != GetHeight())
    {
        m_height = height;
        UpdatePxShape();
    }
}

void CapsuleCollider::SetAxis(Axis3D axis)
{
    if (axis != GetAxis())
    {
        m_axis = axis;
        UpdatePxShape();
    }
}

Axis3D CapsuleCollider::GetAxis() const
{
    return m_axis;
}

float CapsuleCollider::GetRadius() const
{
    return m_radius;
}

float CapsuleCollider::GetHeight() const
{
    return m_height;
}

float CapsuleCollider::GetScaledHeight() const
{
    Vector3 scale = GetGameObject()->GetTransform()->GetScale();
    return scale.y * GetHeight();
}

float CapsuleCollider::GetScaledRadius() const
{
    Vector3 scale = GetGameObject()->GetTransform()->GetScale();
    float maxScaleXZ = Math::Max(scale.x, scale.z);
    return maxScaleXZ * GetRadius();
}

void CapsuleCollider::Reflect()
{
    Collider::Reflect();

    BANG_REFLECT_VAR_MEMBER_HINTED(CapsuleCollider,
                                   "Radius",
                                   SetRadius,
                                   GetRadius,
                                   BANG_REFLECT_HINT_MIN_VALUE(0.0001f));
    BANG_REFLECT_VAR_MEMBER_HINTED(CapsuleCollider,
                                   "Height",
                                   SetHeight,
                                   GetHeight,
                                   BANG_REFLECT_HINT_MIN_VALUE(0.0001f));

    BANG_REFLECT_VAR_ENUM("Axis", SetAxis, GetAxis, Axis3D);
    BANG_REFLECT_HINT_ENUM_FIELD_VALUE("Axis", "X", Axis3D::X);
    BANG_REFLECT_HINT_ENUM_FIELD_VALUE("Axis", "Y", Axis3D::Y);
    BANG_REFLECT_HINT_ENUM_FIELD_VALUE("Axis", "Z", Axis3D::Z);
}

physx::PxShape *CapsuleCollider::CreatePxShape() const
{
    return GetPxRigidDynamic()
               ? GetPxRigidDynamic()->createShape(
                     physx::PxCapsuleGeometry(1, 1),
                     *Physics::GetDefaultPxMaterial())
               : nullptr;
}

Quaternion CapsuleCollider::GetInternalRotation() const
{
    switch (GetAxis())
    {
        case Axis3D::X: return Quaternion::Identity(); break;

        case Axis3D::Y:
            return Quaternion::AngleAxis(Math::Pi * 0.5f, Vector3::Forward());
            break;

        case Axis3D::Z:
            return Quaternion::AngleAxis(Math::Pi * 0.5f, Vector3::Up());
            break;
    }
    return Quaternion::Identity();
}

void CapsuleCollider::UpdatePxShape()
{
    Collider::UpdatePxShape();

    if (GetPxShape())
    {
        float scaledRadius = GetScaledRadius();
        float scaledHeight = GetScaledHeight();
        physx::PxCapsuleGeometry capsuleGeometry;
        capsuleGeometry.radius = scaledRadius;
        capsuleGeometry.halfHeight = scaledHeight * 0.5f;
        GetPxShape()->setGeometry(capsuleGeometry);

        if (GetPxRigidDynamic())
        {
            physx::PxRigidBodyExt::updateMassAndInertia(*GetPxRigidDynamic(),
                                                        1.0f);
        }
    }
}
