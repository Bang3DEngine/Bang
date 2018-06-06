#include "Bang/MaterialFactory.h"

#include "Bang/Paths.h"
#include "Bang/Resources.h"

USING_NAMESPACE_BANG

RH<Material> MaterialFactory::GetDefault(RenderPass renderPass)
{
    switch (renderPass)
    {
        case RenderPass::SCENE:
            return MaterialFactory::Load("Materials/Default.bmat");
        break;

        case RenderPass::SCENE_TRANSPARENT:
            return MaterialFactory::Load("Materials/DefaultTransparent.bmat");
        break;

        default: break;
    }
    return MaterialFactory::GetDefault(RenderPass::SCENE);
}
RH<Material> MaterialFactory::GetDefaultUnLighted()
{
    return MaterialFactory::Load("Materials/DefaultUnLighted.bmat");
}
RH<Material> MaterialFactory::GetGizmosUnLightedOverlay()
{
    return MaterialFactory::Load("Materials/GizmosUnLightedOverlay.bmat");
}
RH<Material> MaterialFactory::GetMissing()
{
    return MaterialFactory::Load("Materials/Missing.bmat");
}

RH<Material> MaterialFactory::GetUIText()
{
    return MaterialFactory::Load("Materials/UITextRenderer.bmat");
}
RH<Material> MaterialFactory::GetUIImage()
{
    return MaterialFactory::Load("Materials/UIImageRenderer.bmat");
}

RH<Material> MaterialFactory::Load(const String &matEnginePath)
{
    MaterialFactory *mf = MaterialFactory::GetActive();
    if (!mf->m_cache.ContainsKey(matEnginePath))
    {
        mf->m_cache.Add(matEnginePath,
                        Resources::Load<Material>(EPATH(matEnginePath)));
    }
    return mf->m_cache.Get(matEnginePath);
}

MaterialFactory *MaterialFactory::GetActive()
{
    return Resources::GetInstance()->GetMaterialFactory();
}
