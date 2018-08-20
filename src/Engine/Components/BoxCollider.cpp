#include "Bang/BoxCollider.h"

#include "Bang/Physics.h"
#include "Bang/Transform.h"

USING_NAMESPACE_BANG

BoxCollider::BoxCollider()
{
    SetPhysicsObjectType( PhysicsObject::Type::BOX_COLLIDER );
}

BoxCollider::~BoxCollider()
{
}

void BoxCollider::CloneInto(ICloneable *clone) const
{
    Collider::CloneInto(clone);
}

void BoxCollider::ImportXML(const XMLNode &xmlInfo)
{
    Collider::ImportXML(xmlInfo);
}

void BoxCollider::ExportXML(XMLNode *xmlInfo) const
{
    Collider::ExportXML(xmlInfo);
}

void BoxCollider::UpdateShapeGeometry()
{
    if (GetPxShape())
    {
        ASSERT(GetPxRigidBody());
        ASSERT(GetPxShape());

        Vector3 halfExtents = 0.5f * GetGameObject()->GetTransform()->GetScale();
        physx::PxBoxGeometry boxGeometry;
        boxGeometry.halfExtents = Physics::GetPxVec3FromVector3( halfExtents );
        GetPxShape()->setGeometry(boxGeometry);

        physx::PxRigidBodyExt::updateMassAndInertia(*GetPxRigidBody(), 1.0f);
    }
}
