#include "Bang/MaterialFactory.h"

#include "Bang/Assets.h"
#include "Bang/Assets.tcc"
#include "Bang/Material.h"
#include "Bang/Paths.h"
#include "Bang/PhysicsMaterial.h"

using namespace Bang;

AH<Material> MaterialFactory::GetDefault(RenderPass renderPass)
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

AH<Material> MaterialFactory::GetGizmosUnLightedOverlay()
{
    return MaterialFactory::LoadMaterial("GizmosUnLightedOverlay.bmat");
}

AH<Material> MaterialFactory::GetParticlesAdditive()
{
    return MaterialFactory::LoadMaterial("ParticlesAdditive.bmat");
}
AH<Material> MaterialFactory::GetParticlesMesh()
{
    return MaterialFactory::LoadMaterial("ParticlesMesh.bmat");
}
AH<Bang::Material> Bang::MaterialFactory::GetWater()
{
    return MaterialFactory::LoadMaterial("Water.bmat");
}

AH<PhysicsMaterial> MaterialFactory::GetDefaultPhysicsMaterial()
{
    return MaterialFactory::LoadPhysicsMaterial("Default.bphmat");
}
AH<Material> MaterialFactory::GetMissing()
{
    return MaterialFactory::LoadMaterial("Missing.bmat");
}

AH<Material> MaterialFactory::GetUIText()
{
    return MaterialFactory::LoadMaterial("UITextRenderer.bmat");
}
AH<Material> MaterialFactory::GetUIImage()
{
    return MaterialFactory::LoadMaterial("UIImageRenderer.bmat");
}

AH<Material> MaterialFactory::LoadMaterial(const String &matEnginePathStr)
{
    Path matEnginePath = Paths::GetEngineAssetsDir()
                             .Append("Materials")
                             .Append(matEnginePathStr);
    MaterialFactory *mf = MaterialFactory::GetActive();
    if (!mf->m_cacheMaterials.ContainsKey(matEnginePath))
    {
        mf->m_cacheMaterials.Add(matEnginePath,
                                 Assets::Load<Material>(matEnginePath));
    }
    return mf->m_cacheMaterials.Get(matEnginePath);
}

AH<PhysicsMaterial> MaterialFactory::LoadPhysicsMaterial(
    const String &phMatEnginePathStr)
{
    Path phMatEnginePath = Paths::GetEngineAssetsDir()
                               .Append("Materials")
                               .Append(phMatEnginePathStr);
    MaterialFactory *mf = MaterialFactory::GetActive();
    if (!mf->m_cachePhysicsMaterials.ContainsKey(phMatEnginePath))
    {
        mf->m_cachePhysicsMaterials.Add(
            phMatEnginePath, Assets::Load<PhysicsMaterial>(phMatEnginePath));
    }
    return mf->m_cachePhysicsMaterials.Get(phMatEnginePath);
}

MaterialFactory *MaterialFactory::GetActive()
{
    return Assets::GetInstance()->GetMaterialFactory();
}
