#ifndef PHYSICSMATERIAL_H
#define PHYSICSMATERIAL_H

#include "Bang/Asset.h"
#include "Bang/BangDefines.h"
#include "Bang/ICloneable.h"
#include "Bang/MetaNode.h"
#include "Bang/String.h"
#include "PxMaterial.h"

NAMESPACE_BANG_BEGIN

FORWARD class Path;

class PhysicsMaterial : public Asset,
                        public ICloneable
{
    ASSET(PhysicsMaterial)
    ICLONEABLE(PhysicsMaterial)

public:
    enum class CombineMode
    {
        AVERAGE  = physx::PxCombineMode::eAVERAGE,
        MIN      = physx::PxCombineMode::eMIN,
        MULTIPLY = physx::PxCombineMode::eMULTIPLY,
        MAX      = physx::PxCombineMode::eMAX
    };

	PhysicsMaterial();
	virtual ~PhysicsMaterial() override;

    void SetStaticFriction(float staticFrictionCoeff);
    void SetDynamicFriction(float dynamicFrictionCoeff);
    void SetRestitution(float restitutionCoeff);
    void SetFrictionCombineMode(PhysicsMaterial::CombineMode combineMode);
    void SetRestitutionCombineMode(PhysicsMaterial::CombineMode combineMode);

    float GetStaticFriction() const;
    float GetDynamicFriction() const;
    float GetRestitution() const;
    CombineMode GetFrictionCombineMode() const;
    CombineMode GetRestitutionCombineMode() const;

    // ICloneable
    virtual void CloneInto(ICloneable *clone) const override;

    // Resource
    void Import(const Path &physicsMaterialFilepath) override;

    // Serializable
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;

private:
    physx::PxMaterial *p_pxMaterial = nullptr;

    float m_staticFriction = 0.5f;
    float m_dynamicFriction = 0.5f;
    float m_restitution = 0.5f;
    CombineMode m_frictionCombineMode = CombineMode::AVERAGE;
    CombineMode m_restitutionCombineMode = CombineMode::AVERAGE;

    void SetPxMaterial(physx::PxMaterial *pxMaterial);
    physx::PxMaterial* GetPxMaterial() const;

    void UpdatePxMaterial() const;

    friend class Physics;
    friend class Collider;
    friend class PxSceneContainer;
};

NAMESPACE_BANG_END

#endif // PHYSICSMATERIAL_H

