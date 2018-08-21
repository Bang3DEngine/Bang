#include "Bang/RigidBody.h"

#include "Bang/Physics.h"
#include "Bang/XMLNode.h"

USING_NAMESPACE_BANG

RigidBody::RigidBody()
{
    SetPhysicsObjectType( PhysicsObject::Type::RIGIDBODY );
}

RigidBody::~RigidBody()
{
}

void RigidBody::SetMass(float mass)
{
    if (mass != GetMass())
    {
        m_mass = mass;
        UpdatePxRigidDynamicValues();
    }
}

void RigidBody::SetDrag(float drag)
{
    if (drag != GetDrag())
    {
        m_drag = drag;
        UpdatePxRigidDynamicValues();
    }
}

void RigidBody::SetAngularDrag(float angularDrag)
{
    if (angularDrag != GetAngularDrag())
    {
        m_angularDrag = angularDrag;
        UpdatePxRigidDynamicValues();
    }
}

void RigidBody::SetUseGravity(bool useGravity)
{
    if (useGravity != GetUseGravity())
    {
        m_useGravity = useGravity;
        UpdatePxRigidDynamicValues();
    }
}

void RigidBody::SetIsKinematic(bool isKinematic)
{
    if (isKinematic != GetIsKinematic())
    {
        m_isKinematic = isKinematic;
        UpdatePxRigidDynamicValues();
    }
}

float RigidBody::GetMass() const
{
    return m_mass;
}

float RigidBody::GetDrag() const
{
    return m_drag;
}

float RigidBody::GetAngularDrag() const
{
    return m_angularDrag;
}

bool RigidBody::GetUseGravity() const
{
    return m_useGravity;
}

bool RigidBody::GetIsKinematic() const
{
    return m_isKinematic;
}

void RigidBody::CloneInto(ICloneable *clone) const
{
    Component::CloneInto(clone);

    RigidBody *rbClone = SCAST<RigidBody*>(clone);
    rbClone->SetMass( GetMass() );
    rbClone->SetDrag( GetDrag() );
    rbClone->SetAngularDrag( GetAngularDrag() );
    rbClone->SetUseGravity( GetUseGravity() );
    rbClone->SetIsKinematic( GetIsKinematic() );
}

void RigidBody::ImportXML(const XMLNode &xmlInfo)
{
    Component::ImportXML(xmlInfo);

    if (xmlInfo.Contains("Mass"))
    {
        SetMass( xmlInfo.Get<float>("Mass") );
    }

    if (xmlInfo.Contains("Drag"))
    {
        SetDrag( xmlInfo.Get<float>("Drag") );
    }

    if (xmlInfo.Contains("AngularDrag"))
    {
        SetAngularDrag( xmlInfo.Get<float>("AngularDrag") );
    }

    if (xmlInfo.Contains("UseGravity"))
    {
        SetUseGravity( xmlInfo.Get<bool>("UseGravity") );
    }

    if (xmlInfo.Contains("IsKinematic"))
    {
        SetIsKinematic( xmlInfo.Get<bool>("IsKinematic") );
    }
}

void RigidBody::ExportXML(XMLNode *xmlInfo) const
{
    Component::ExportXML(xmlInfo);

    xmlInfo->Set("Mass",        GetMass());
    xmlInfo->Set("Drag",        GetDrag());
    xmlInfo->Set("AngularDrag", GetAngularDrag());
    xmlInfo->Set("UseGravity",  GetUseGravity());
    xmlInfo->Set("IsKinematic", GetIsKinematic());
}

void RigidBody::UpdatePxRigidDynamicValues()
{
    if (GetPxRigidDynamic())
    {
        GetPxRigidDynamic()->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY,
                                          !GetUseGravity());
        GetPxRigidDynamic()->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC,
                                              GetIsKinematic());
        GetPxRigidDynamic()->setMass( GetMass() );
        GetPxRigidDynamic()->setLinearDamping( GetDrag() );
        GetPxRigidDynamic()->setAngularDamping( GetAngularDrag() );
    }
}

void RigidBody::SetPxRigidDynamic(physx::PxRigidDynamic *pxRigidDynamic)
{
    if (pxRigidDynamic != GetPxRigidDynamic())
    {
        p_pxRigidDynamic = pxRigidDynamic;
        UpdatePxRigidDynamicValues();
    }
}

physx::PxRigidDynamic *RigidBody::GetPxRigidDynamic() const
{
    return p_pxRigidDynamic;
}

