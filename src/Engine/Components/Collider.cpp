#include "Bang/Collider.h"

#include "PxPhysicsAPI.h"

#include "Bang/Physics.h"
#include "Bang/XMLNode.h"
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

void Collider::SetCenter(const Vector3 &center)
{
    if (center != GetCenter())
    {
        m_center = center;
        UpdateShapeGeometry();
    }
}

const Vector3 &Collider::GetCenter() const
{
    return m_center;
}

void Collider::OnEnabled(Object *)
{
    GetPxShape()->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, true);
    GetPxShape()->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, false);
}

void Collider::OnDisabled(Object *)
{
    GetPxShape()->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, false);
    GetPxShape()->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, false);
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

    Collider *colliderClone = SCAST<Collider*>(clone);
    colliderClone->SetCenter( GetCenter() );
}

void Collider::ImportXML(const XMLNode &xmlInfo)
{
    Component::ImportXML(xmlInfo);

    if (xmlInfo.Contains("Center"))
    {
        SetCenter( xmlInfo.Get<Vector3>("Center") );
    }
}

void Collider::ExportXML(XMLNode *xmlInfo) const
{
    Component::ExportXML(xmlInfo);

    xmlInfo->Set("Center", GetCenter());
}

void Collider::UpdateShapeGeometry()
{
    if (GetPxShape())
    {
        physx::PxTransform pxLocalTransform = GetPxShape()->getLocalPose();
        pxLocalTransform.p = Physics::GetPxVec3FromVector3( GetCenter() );
        GetPxShape()->setLocalPose( pxLocalTransform );

        physx::PxRigidBodyExt::updateMassAndInertia(*GetPxRigidBody(), 1.0f);
    }
}
