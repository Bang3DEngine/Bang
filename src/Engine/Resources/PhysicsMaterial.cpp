#include "Bang/PhysicsMaterial.h"

#include "Bang/Physics.h"
#include "Bang/XMLNode.h"

USING_NAMESPACE_BANG

PhysicsMaterial::PhysicsMaterial()
{
    Physics::GetInstance()->RegisterPhysicsMaterial(this);
}

PhysicsMaterial::~PhysicsMaterial()
{
    GetPxMaterial()->release();
}

void PhysicsMaterial::SetStaticFriction(float staticFrictionCoeff)
{
    if (staticFrictionCoeff != GetStaticFriction())
    {
        m_staticFriction = staticFrictionCoeff;
        UpdatePxMaterial();
    }
}

void PhysicsMaterial::SetDynamicFriction(float dynamicFrictionCoeff)
{
    if (dynamicFrictionCoeff != GetDynamicFriction())
    {
        m_dynamicFriction = dynamicFrictionCoeff;
        UpdatePxMaterial();
    }
}

void PhysicsMaterial::SetRestitution(float restitutionCoeff)
{
    if (restitutionCoeff != GetRestitution())
    {
        m_restitution = restitutionCoeff;
        UpdatePxMaterial();
    }
}

void PhysicsMaterial::SetFrictionCombineMode(CombineMode combineMode)
{
    if (combineMode != GetFrictionCombineMode())
    {
        m_frictionCombineMode = combineMode;
        UpdatePxMaterial();
    }
}

void PhysicsMaterial::SetRestitutionCombineMode(CombineMode combineMode)
{
    if (combineMode != GetRestitutionCombineMode())
    {
        m_restitutionCombineMode = combineMode;
        UpdatePxMaterial();
    }
}

float PhysicsMaterial::GetStaticFriction() const
{
    return m_staticFriction;
}

float PhysicsMaterial::GetDynamicFriction() const
{
    return m_dynamicFriction;
}

float PhysicsMaterial::GetRestitution() const
{
    return m_restitution;
}

PhysicsMaterial::CombineMode PhysicsMaterial::GetFrictionCombineMode() const
{
    return m_frictionCombineMode;
}

PhysicsMaterial::CombineMode PhysicsMaterial::GetRestitutionCombineMode() const
{
    return m_restitutionCombineMode;
}

void PhysicsMaterial::CloneInto(ICloneable *clone) const
{
    PhysicsMaterial *pMat = SCAST<PhysicsMaterial*>(clone);
    pMat->SetStaticFriction( GetStaticFriction() );
    pMat->SetDynamicFriction( GetDynamicFriction() );
    pMat->SetRestitution( GetRestitution() );
    pMat->SetFrictionCombineMode( GetFrictionCombineMode() );
    pMat->SetRestitutionCombineMode( GetRestitutionCombineMode() );
}

void PhysicsMaterial::Import(const Path &physicsMaterialFilepath)
{
    ImportXMLFromFile(physicsMaterialFilepath);
}

void PhysicsMaterial::ImportXML(const XMLNode &xmlInfo)
{
    Asset::ImportXML(xmlInfo);

    if (xmlInfo.Contains("StaticFriction"))
    {
        SetStaticFriction( xmlInfo.Get<float>("StaticFriction") );
    }

    if (xmlInfo.Contains("DynamicFriction"))
    {
        SetDynamicFriction( xmlInfo.Get<float>("DynamicFriction") );
    }

    if (xmlInfo.Contains("Restitution"))
    {
        SetRestitution( xmlInfo.Get<float>("Restitution") );
    }

    if (xmlInfo.Contains("FrictionCombineMode"))
    {
        SetFrictionCombineMode(
                    xmlInfo.Get<CombineMode>("FrictionCombineMode") );
    }

    if (xmlInfo.Contains("RestitutionCombineMode"))
    {
        SetRestitutionCombineMode(
                    xmlInfo.Get<CombineMode>("RestitutionCombineMode") );
    }
}

void PhysicsMaterial::ExportXML(XMLNode *xmlInfo) const
{
    Asset::ExportXML(xmlInfo);

    xmlInfo->Set("StaticFriction", GetStaticFriction());
    xmlInfo->Set("DynamicFriction", GetDynamicFriction());
    xmlInfo->Set("Restitution", GetRestitution());
    xmlInfo->Set("FrictionCombineMode",
                 SCAST<int>(GetFrictionCombineMode()));
    xmlInfo->Set("RestitutionCombineMode",
                 SCAST<int>(GetRestitutionCombineMode()));
}

void PhysicsMaterial::SetPxMaterial(physx::PxMaterial *pxMaterial)
{
    p_pxMaterial = pxMaterial;
}

physx::PxMaterial* PhysicsMaterial::GetPxMaterial() const
{
    return p_pxMaterial;
}

void PhysicsMaterial::UpdatePxMaterial() const
{
    if (GetPxMaterial())
    {
        GetPxMaterial()->setStaticFriction( GetStaticFriction() );
        GetPxMaterial()->setDynamicFriction( GetDynamicFriction() );
        GetPxMaterial()->setRestitution( GetRestitution() );
        GetPxMaterial()->setFrictionCombineMode(
            SCAST<physx::PxCombineMode::Enum>( GetFrictionCombineMode()) );
        GetPxMaterial()->setRestitutionCombineMode(
            SCAST<physx::PxCombineMode::Enum>( GetRestitutionCombineMode()) );
    }
}

