#include "Bang/PhysicsMaterial.h"

#include "Bang/MetaNode.h"
#include "Bang/MetaNode.tcc"
#include "Bang/Physics.h"

namespace Bang
{
class Path;
}

using namespace Bang;

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

void PhysicsMaterial::CloneInto(ICloneable *clone, bool cloneGUID) const
{
    Serializable::CloneInto(clone, cloneGUID);

    PhysicsMaterial *pMat = SCAST<PhysicsMaterial *>(clone);
    pMat->SetStaticFriction(GetStaticFriction());
    pMat->SetDynamicFriction(GetDynamicFriction());
    pMat->SetRestitution(GetRestitution());
    pMat->SetFrictionCombineMode(GetFrictionCombineMode());
    pMat->SetRestitutionCombineMode(GetRestitutionCombineMode());
}

void PhysicsMaterial::Import(const Path &physicsMaterialFilepath)
{
    ImportMetaFromFile(physicsMaterialFilepath);
}

void PhysicsMaterial::ImportMeta(const MetaNode &metaNode)
{
    Asset::ImportMeta(metaNode);

    if (metaNode.Contains("StaticFriction"))
    {
        SetStaticFriction(metaNode.Get<float>("StaticFriction"));
    }

    if (metaNode.Contains("DynamicFriction"))
    {
        SetDynamicFriction(metaNode.Get<float>("DynamicFriction"));
    }

    if (metaNode.Contains("Restitution"))
    {
        SetRestitution(metaNode.Get<float>("Restitution"));
    }

    if (metaNode.Contains("FrictionCombineMode"))
    {
        SetFrictionCombineMode(
            metaNode.Get<CombineMode>("FrictionCombineMode"));
    }

    if (metaNode.Contains("RestitutionCombineMode"))
    {
        SetRestitutionCombineMode(
            metaNode.Get<CombineMode>("RestitutionCombineMode"));
    }
}

void PhysicsMaterial::ExportMeta(MetaNode *metaNode) const
{
    Asset::ExportMeta(metaNode);

    metaNode->Set("StaticFriction", GetStaticFriction());
    metaNode->Set("DynamicFriction", GetDynamicFriction());
    metaNode->Set("Restitution", GetRestitution());
    metaNode->Set("FrictionCombineMode", SCAST<int>(GetFrictionCombineMode()));
    metaNode->Set("RestitutionCombineMode",
                  SCAST<int>(GetRestitutionCombineMode()));
}

void PhysicsMaterial::SetPxMaterial(physx::PxMaterial *pxMaterial)
{
    p_pxMaterial = pxMaterial;
}

physx::PxMaterial *PhysicsMaterial::GetPxMaterial() const
{
    return p_pxMaterial;
}

void PhysicsMaterial::UpdatePxMaterial() const
{
    if (GetPxMaterial())
    {
        GetPxMaterial()->setStaticFriction(GetStaticFriction());
        GetPxMaterial()->setDynamicFriction(GetDynamicFriction());
        GetPxMaterial()->setRestitution(GetRestitution());
        GetPxMaterial()->setFrictionCombineMode(
            SCAST<physx::PxCombineMode::Enum>(GetFrictionCombineMode()));
        GetPxMaterial()->setRestitutionCombineMode(
            SCAST<physx::PxCombineMode::Enum>(GetRestitutionCombineMode()));
    }
}
