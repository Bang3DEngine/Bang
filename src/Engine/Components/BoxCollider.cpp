#include "Bang/BoxCollider.h"

#include "Bang/Physics.h"
#include "Bang/XMLNode.h"
#include "Bang/Transform.h"
#include "Bang/GameObject.h"

USING_NAMESPACE_BANG

BoxCollider::BoxCollider()
{
    SetPhysicsObjectType( PhysicsObject::Type::BOX_COLLIDER );
}

BoxCollider::~BoxCollider()
{
}

void BoxCollider::SetHalfExtents(const Vector3 &halfExtents)
{
    if (halfExtents != GetHalfExtents())
    {
        m_halfExtents = halfExtents;
        UpdatePxShape();
    }
}

const Vector3 &BoxCollider::GetHalfExtents() const
{
    return m_halfExtents;
}

void BoxCollider::CloneInto(ICloneable *clone) const
{
    Collider::CloneInto(clone);

    BoxCollider *bcClone = SCAST<BoxCollider*>(clone);
    bcClone->SetHalfExtents( GetHalfExtents() );
}

void BoxCollider::ImportXML(const XMLNode &xmlInfo)
{
    Collider::ImportXML(xmlInfo);

    if (xmlInfo.Contains("HalfExtents"))
    {
        SetHalfExtents( xmlInfo.Get<Vector3>("HalfExtents") );
    }
}

void BoxCollider::ExportXML(XMLNode *xmlInfo) const
{
    Collider::ExportXML(xmlInfo);

    xmlInfo->Set("HalfExtents", GetHalfExtents());
}

void BoxCollider::UpdatePxShape()
{
    Collider::UpdatePxShape();

    if (GetPxShape())
    {
        ASSERT(GetPxRigidBody());
        ASSERT(GetPxShape());

        Vector3 halfExtents = GetHalfExtents() *
                              GetGameObject()->GetTransform()->GetScale();
        physx::PxBoxGeometry boxGeometry;
        boxGeometry.halfExtents = Physics::GetPxVec3FromVector3( halfExtents );
        GetPxShape()->setGeometry(boxGeometry);

        physx::PxRigidBodyExt::updateMassAndInertia(*GetPxRigidBody(), 1.0f);
    }
}
