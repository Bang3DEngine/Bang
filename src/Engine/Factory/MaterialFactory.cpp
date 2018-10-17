#include "Bang/MaterialFactory.h"

#include "Bang/Material.h"
#include "Bang/Paths.h"
#include "Bang/PhysicsMaterial.h"
#include "Bang/Resources.h"
#include "Bang/Resources.tcc"
#include "Bang/UMap.tcc"

using namespace Bang;

RH<Material> MaterialFactory::GetDefault(RenderPass renderPass)
{
    switch (renderPass)
    {
        case RenderPass::SCENE:
            return MaterialFactory::LoadMaterial("Materials/Default.bmat");

        case RenderPass::SCENE_TRANSPARENT:
            return MaterialFactory::LoadMaterial(
                "Materials/DefaultTransparent.bmat");

        default: break;
    }
    return MaterialFactory::GetDefault(RenderPass::SCENE);
}

RH<Material> MaterialFactory::GetDefaultUnLighted()
{
    return MaterialFactory::LoadMaterial("Materials/DefaultUnLighted.bmat");
}
RH<Material> MaterialFactory::GetGizmosUnLightedOverlay()
{
    return MaterialFactory::LoadMaterial(
        "Materials/GizmosUnLightedOverlay.bmat");
}

RH<Material> MaterialFactory::GetParticlesAdditive()
{
    return MaterialFactory::LoadMaterial("Materials/ParticlesAdditive.bmat");
}
RH<Material> MaterialFactory::GetParticlesMesh()
{
    return MaterialFactory::LoadMaterial("Materials/ParticlesMesh.bmat");
}
RH<Bang::Material> Bang::MaterialFactory::GetWater()
{
    return MaterialFactory::LoadMaterial("Materials/Water.bmat");
}

RH<PhysicsMaterial> MaterialFactory::GetDefaultPhysicsMaterial()
{
    return MaterialFactory::LoadPhysicsMaterial("Materials/Default.bphmat");
}
RH<Material> MaterialFactory::GetMissing()
{
    return MaterialFactory::LoadMaterial("Materials/Missing.bmat");
}

RH<Material> MaterialFactory::GetUIText()
{
    return MaterialFactory::LoadMaterial("Materials/UITextRenderer.bmat");
}
RH<Material> MaterialFactory::GetUIImage()
{
    return MaterialFactory::LoadMaterial("Materials/UIImageRenderer.bmat");
}

RH<Material> MaterialFactory::LoadMaterial(const String &matEnginePath)
{
    MaterialFactory *mf = MaterialFactory::GetActive();
    if (!mf->m_cacheMaterials.ContainsKey(matEnginePath))
    {
        mf->m_cacheMaterials.Add(
            matEnginePath, Resources::Load<Material>(EPATH(matEnginePath)));
    }
    return mf->m_cacheMaterials.Get(matEnginePath);
}

RH<PhysicsMaterial> MaterialFactory::LoadPhysicsMaterial(
    const String &phMatEnginePath)
{
    MaterialFactory *mf = MaterialFactory::GetActive();
    if (!mf->m_cachePhysicsMaterials.ContainsKey(phMatEnginePath))
    {
        mf->m_cachePhysicsMaterials.Add(
            phMatEnginePath,
            Resources::Load<PhysicsMaterial>(EPATH(phMatEnginePath)));
    }
    return mf->m_cachePhysicsMaterials.Get(phMatEnginePath);
}

MaterialFactory *MaterialFactory::GetActive()
{
    return Resources::GetInstance()->GetMaterialFactory();
}
