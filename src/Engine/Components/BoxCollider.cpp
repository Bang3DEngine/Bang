#include "Bang/BoxCollider.h"

#include "Bang/Assert.h"
#include "Bang/FastDynamicCast.h"
#include "Bang/GameObject.h"
#include "Bang/StreamOperators.h"
#include "Bang/MaterialFactory.h"
#include "Bang/PhysicsMaterial.h"
#include "Bang/MetaNode.h"
#include "Bang/MetaNode.tcc"
#include "Bang/Physics.h"
#include "Bang/PhysicsObject.h"
#include "Bang/ResourceHandle.h"
#include "Bang/Transform.h"
#include "Bang/Vector.tcc"
#include "Bang/Vector3.h"
#include "PxRigidDynamic.h"
#include "PxShape.h"
#include "extensions/PxRigidBodyExt.h"
#include "foundation/PxVec3.h"
#include "geometry/PxBoxGeometry.h"

FORWARD NAMESPACE_BANG_BEGIN
FORWARD class ICloneable;
FORWARD NAMESPACE_BANG_END

USING_NAMESPACE_BANG

BoxCollider::BoxCollider()
{
    CONSTRUCT_CLASS_ID(BoxCollider)
    SetPhysicsObjectType( PhysicsObject::Type::BOX_COLLIDER );
    SetPhysicsMaterial( MaterialFactory::GetDefaultPhysicsMaterial().Get() );
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
    box.SetCenter( tr->GetPosition() + GetCenter() );
    box.SetOrientation( tr->GetRotation() );
    box.SetLocalExtents( tr->GetScale() * GetExtents() );

    return box;
}

const Vector3 &BoxCollider::GetExtents() const
{
    return m_extents;
}

void BoxCollider::CloneInto(ICloneable *clone) const
{
    Collider::CloneInto(clone);

    BoxCollider *bcClone = SCAST<BoxCollider*>(clone);
    bcClone->SetExtents( GetExtents() );
}

void BoxCollider::ImportMeta(const MetaNode &metaNode)
{
    Collider::ImportMeta(metaNode);

    if (metaNode.Contains("Extents"))
    {
        SetExtents( metaNode.Get<Vector3>("Extents") );
    }
}

void BoxCollider::ExportMeta(MetaNode *metaNode) const
{
    Collider::ExportMeta(metaNode);

    metaNode->Set("Extents", GetExtents());
}

physx::PxShape *BoxCollider::CreatePxShape() const
{
    return GetPxRigidDynamic() ?
            GetPxRigidDynamic()->createShape(physx::PxBoxGeometry(1, 1, 1),
                                             *Physics::GetDefaultPxMaterial()) :
            nullptr;
}

void BoxCollider::UpdatePxShape()
{
    Collider::UpdatePxShape();

    if (GetPxShape())
    {
        ASSERT(GetPxRigidDynamic());

        Transform *tr = GetGameObject()->GetTransform();
        Vector3 extents = GetExtents() * tr->GetScale();
        physx::PxBoxGeometry boxGeometry;
        boxGeometry.halfExtents = Physics::GetPxVec3FromVector3( extents );
        GetPxShape()->setGeometry(boxGeometry);

        physx::PxRigidBodyExt::updateMassAndInertia(*GetPxRigidDynamic(), 1.0f);
    }
}
