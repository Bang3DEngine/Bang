#include "Bang/BoxCollider.h"

#include "Bang/Physics.h"
#include "Bang/MetaNode.h"
#include "Bang/Transform.h"
#include "Bang/GameObject.h"

USING_NAMESPACE_BANG

BoxCollider::BoxCollider()
{
    CONSTRUCT_CLASS_ID(BoxCollider)
    SetPhysicsObjectType( PhysicsObject::Type::BOX_COLLIDER );
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

void BoxCollider::UpdatePxShape()
{
    Collider::UpdatePxShape();

    if (GetPxShape())
    {
        ASSERT(GetPxRigidBody());
        ASSERT(GetPxShape());

        Transform *tr = GetGameObject()->GetTransform();
        Vector3 extents = GetExtents() * tr->GetScale();
        physx::PxBoxGeometry boxGeometry;
        boxGeometry.halfExtents = Physics::GetPxVec3FromVector3( extents );
        GetPxShape()->setGeometry(boxGeometry);

        physx::PxRigidBodyExt::updateMassAndInertia(*GetPxRigidBody(), 1.0f);
    }
}
