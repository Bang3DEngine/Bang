#include "Bang/Collider.h"

#include "PxPhysicsAPI.h"

#include "Bang/Physics.h"
#include "Bang/XMLNode.h"
#include "Bang/Resources.h"
#include "Bang/Transform.h"
#include "Bang/PhysicsMaterial.h"
#include "Bang/MaterialFactory.h"

USING_NAMESPACE_BANG

Collider::Collider()
{
    SetPhysicsMaterial( MaterialFactory::GetDefaultPhysicsMaterial().Get() );
}

Collider::~Collider()
{
}

void Collider::OnUpdate()
{
    Component::OnUpdate();
    UpdatePxShape();
}

void Collider::SetIsTrigger(bool isTrigger)
{
    if (isTrigger != GetIsTrigger())
    {
        m_isTrigger = isTrigger;
        UpdatePxShape();
    }
}

void Collider::SetCenter(const Vector3 &center)
{
    if (center != GetCenter())
    {
        m_center = center;
        UpdatePxShape();
    }
}

void Collider::SetPhysicsMaterial(PhysicsMaterial *physicsMaterial)
{
    if (physicsMaterial != GetSharedPhysicsMaterial())
    {
        if (p_physicsMaterial.Get())
        {
            p_physicsMaterial.Set(nullptr);
        }

        p_sharedPhysicsMaterial.Set(physicsMaterial);
        UpdatePxShape();
    }
}

bool Collider::GetIsTrigger() const
{
    return m_isTrigger;
}

const Vector3 &Collider::GetCenter() const
{
    return m_center;
}

PhysicsMaterial *Collider::GetSharedPhysicsMaterial() const
{
    return p_sharedPhysicsMaterial.Get();
}

PhysicsMaterial *Collider::GetActivePhysicsMaterial() const
{
    if (p_physicsMaterial.Get())
    {
        return GetPhysicsMaterial();
    }
    return GetSharedPhysicsMaterial();
}

PhysicsMaterial *Collider::GetPhysicsMaterial() const
{
    if (!p_physicsMaterial)
    {
        if (GetSharedPhysicsMaterial())
        {
            p_physicsMaterial = Resources::Clone<PhysicsMaterial>(
                                        GetSharedPhysicsMaterial());
        }
    }
    return p_physicsMaterial.Get();
}

void Collider::OnEnabled(Object *)
{
    GetPxShape()->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, true);
    GetPxShape()->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, GetIsTrigger());
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
    colliderClone->SetIsTrigger( GetIsTrigger() );
    colliderClone->SetPhysicsMaterial( GetSharedPhysicsMaterial() );
}

void Collider::ImportXML(const XMLNode &xmlInfo)
{
    Component::ImportXML(xmlInfo);

    if (xmlInfo.Contains("IsTrigger"))
    {
        SetIsTrigger( xmlInfo.Get<bool>("IsTrigger") );
    }

    if (xmlInfo.Contains("Center"))
    {
        SetCenter( xmlInfo.Get<Vector3>("Center") );
    }

    if (xmlInfo.Contains("PhysicsMaterial"))
    {
        RH<PhysicsMaterial> phMat = Resources::Load<PhysicsMaterial>(
                                      xmlInfo.Get<GUID>("PhysicsMaterial"));
        SetPhysicsMaterial(phMat.Get());
    }
}

void Collider::ExportXML(XMLNode *xmlInfo) const
{
    Component::ExportXML(xmlInfo);

    xmlInfo->Set("Center", GetCenter());
    xmlInfo->Set("IsTrigger", GetIsTrigger());
    xmlInfo->Set("PhysicsMaterial",
                 GetSharedPhysicsMaterial() ?
                     GetSharedPhysicsMaterial()->GetGUID() : GUID::Empty());
}

Quaternion Collider::GetInternalRotation() const
{
    return Quaternion::Identity;
}

void Collider::UpdatePxShape()
{
    if (GetPxShape())
    {
        physx::PxTransform pxLocalTransform = GetPxShape()->getLocalPose();
        pxLocalTransform.p = Physics::GetPxVec3FromVector3( GetCenter() );
        pxLocalTransform.q = Physics::GetPxQuatFromQuaternion(
                                                    GetInternalRotation() );
        GetPxShape()->setLocalPose( pxLocalTransform );

        GetPxShape()->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, !GetIsTrigger());
        GetPxShape()->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, GetIsTrigger());

        if (GetActivePhysicsMaterial())
        {
            if (GetPxShape())
            {
                physx::PxMaterial *material = GetActivePhysicsMaterial()->
                                              GetPxMaterial();
                GetPxShape()->setMaterials(&material, 1);
            }
        }

        physx::PxRigidBodyExt::updateMassAndInertia(*GetPxRigidBody(), 1.0f);
    }
}
