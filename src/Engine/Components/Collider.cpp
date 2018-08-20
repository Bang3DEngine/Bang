#include "Bang/Collider.h"

#include "PxPhysicsAPI.h"

#include "Bang/Physics.h"
#include "Bang/Transform.h"

USING_NAMESPACE_BANG

Collider::Collider()
{
}

Collider::~Collider()
{
}

void Collider::OnUpdate()
{
    Component::OnUpdate();

    UpdateShapeGeometry();
}

void Collider::SetPxRigidBody(physx::PxRigidBody *pxRB)
{
    p_pxRigidBody = pxRB;
}

void Collider::SetPxShape(physx::PxShape *pxShape)
{
    p_pxShape = pxShape;
}

physx::PxRigidBody *Collider::GetPxRigidBody() const
{
    return p_pxRigidBody;
}

physx::PxShape *Collider::GetPxShape() const
{
    return p_pxShape;
}

void Collider::CloneInto(ICloneable *clone) const
{
    Component::CloneInto(clone);
}

void Collider::ImportXML(const XMLNode &xmlInfo)
{
    Component::ImportXML(xmlInfo);
}

void Collider::ExportXML(XMLNode *xmlInfo) const
{
    Component::ExportXML(xmlInfo);
}
