#include "Bang/MaterialFactory.h"

#include "Bang/Material.h"
#include "Bang/Paths.h"
#include "Bang/PhysicsMaterial.h"
#include "Bang/Resources.h"
#include "Bang/Resources.tcc"

using namespace Bang;

RH<Material> MaterialFactory::GetDefault(RenderPass renderPass)
{
    switch (renderPass)
    {
        case RenderPass::SCENE_OPAQUE:
            return MaterialFactory::LoadMaterial("Default.bmat");

        case RenderPass::SCENE_TRANSPARENT:
            return MaterialFactory::LoadMaterial("DefaultTransparent.bmat");

        default: break;
    }
    return MaterialFactory::GetDefault(RenderPass::SCENE_OPAQUE);
}

RH<Material> MaterialFactory::GetDefaultUnLighted()
{
    return MaterialFactory::LoadMaterial("DefaultUnLighted.bmat");
}
RH<Material> MaterialFactory::GetGizmosUnLightedOverlay()
{
    return MaterialFactory::LoadMaterial("GizmosUnLightedOverlay.bmat");
}

RH<Material> MaterialFactory::GetParticlesAdditive()
{
    return MaterialFactory::LoadMaterial("ParticlesAdditive.bmat");
}
RH<Material> MaterialFactory::GetParticlesMesh()
{
    return MaterialFactory::LoadMaterial("ParticlesMesh.bmat");
}
RH<Bang::Material> Bang::MaterialFactory::GetWater()
{
    return MaterialFactory::LoadMaterial("Water.bmat");
}

RH<PhysicsMaterial> MaterialFactory::GetDefaultPhysicsMaterial()
{
    return MaterialFactory::LoadPhysicsMaterial("Default.bphmat");
}
RH<Material> MaterialFactory::GetMissing()
{
    return MaterialFactory::LoadMaterial("Missing.bmat");
}

RH<Material> MaterialFactory::GetUIText()
{
    return MaterialFactory::LoadMaterial("UITextRenderer.bmat");
}
RH<Material> MaterialFactory::GetUIImage()
{
    return MaterialFactory::LoadMaterial("UIImageRenderer.bmat");
}

RH<Material> MaterialFactory::LoadMaterial(const String &matEnginePathStr)
{
    Path matEnginePath = Paths::GetEngineAssetsDir()
                             .Append("Materials")
                             .Append(matEnginePathStr);
    MaterialFactory *mf = MaterialFactory::GetActive();
    if (!mf->m_cacheMaterials.ContainsKey(matEnginePath))
    {
        mf->m_cacheMaterials.Add(matEnginePath,
                                 Resources::Load<Material>(matEnginePath));
    }
    return mf->m_cacheMaterials.Get(matEnginePath);
}

RH<PhysicsMaterial> MaterialFactory::LoadPhysicsMaterial(
    const String &phMatEnginePathStr)
{
    Path phMatEnginePath = Paths::GetEngineAssetsDir()
                               .Append("Materials")
                               .Append(phMatEnginePathStr);
    MaterialFactory *mf = MaterialFactory::GetActive();
    if (!mf->m_cachePhysicsMaterials.ContainsKey(phMatEnginePath))
    {
        mf->m_cachePhysicsMaterials.Add(
            phMatEnginePath, Resources::Load<PhysicsMaterial>(phMatEnginePath));
    }
    return mf->m_cachePhysicsMaterials.Get(phMatEnginePath);
}

MaterialFactory *MaterialFactory::GetActive()
{
    return Resources::GetInstance()->GetMaterialFactory();
}
