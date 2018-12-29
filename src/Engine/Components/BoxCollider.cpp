#include "Bang/BoxCollider.h"

#include "Bang/Assert.h"
#include "Bang/AssetHandle.h"
#include "Bang/ClassDB.h"
#include "Bang/GameObject.h"
#include "Bang/MaterialFactory.h"
#include "Bang/MetaNode.h"
#include "Bang/MetaNode.tcc"
#include "Bang/Physics.h"
#include "Bang/PhysicsComponent.h"
#include "Bang/PhysicsMaterial.h"
#include "Bang/StreamOperators.h"
#include "Bang/Transform.h"
#include "Bang/Vector3.h"
#include "PxRigidDynamic.h"
#include "PxShape.h"
#include "extensions/PxRigidBodyExt.h"
#include "foundation/PxVec3.h"
#include "geometry/PxBoxGeometry.h"

namespace Bang
{
class ICloneable;
}

using namespace Bang;

BoxCollider::BoxCollider()
{
    SET_INSTANCE_CLASS_ID(BoxCollider)
    SetPhysicsComponentType(PhysicsComponent::Type::BOX_COLLIDER);
    SetPhysicsMaterial(MaterialFactory::GetDefaultPhysicsMaterial().Get());
}

BoxCollider::~BoxCollider()
{
}

void BoxCollider::SetExtents(const Vector3 &extents)
{
    if (extents != GetExtents())
    {
        m_extents = extents;
        UpdatePxShape();
    }
}

Box BoxCollider::GetBoxWorld() const
{
    Transform *tr = GetGameObject()->GetTransform();

    Box box;
    box.SetCenter(tr->GetPosition() + GetCenter());
    box.SetOrientation(tr->GetRotation());
    box.SetLocalExtents(tr->GetScale() * GetExtents());

    return box;
}

const Vector3 &BoxCollider::GetExtents() const
{
    return m_extents;
}

void BoxCollider::Reflect()
{
    Collider::Reflect();

    BANG_REFLECT_VAR_MEMBER_HINTED(BoxCollider,
                                   "Extents",
                                   SetExtents,
                                   GetExtents,
                                   BANG_REFLECT_HINT_MIN_VALUE(0.01f));
}

physx::PxShape *BoxCollider::CreatePxShape() const
{
    return GetPxRigidDynamic()
               ? GetPxRigidDynamic()->createShape(
                     physx::PxBoxGeometry(1, 1, 1),
                     *Physics::GetDefaultPxMaterial())
               : nullptr;
}

void BoxCollider::UpdatePxShape()
{
    Collider::UpdatePxShape();

    if (GetPxShape())
    {
        Transform *tr = GetGameObject()->GetTransform();
        Vector3 extents = GetExtents() * tr->GetScale();
        physx::PxBoxGeometry boxGeometry;
        boxGeometry.halfExtents = Physics::GetPxVec3FromVector3(extents);
        GetPxShape()->setGeometry(boxGeometry);

        if (GetPxRigidDynamic())
        {
            physx::PxRigidBodyExt::updateMassAndInertia(*GetPxRigidDynamic(),
                                                        1.0f);
        }
    }
}
