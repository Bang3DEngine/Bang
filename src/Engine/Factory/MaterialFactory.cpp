#include "Bang/MaterialFactory.h"

#include "Bang/Paths.h"
#include "Bang/Resources.h"

USING_NAMESPACE_BANG

RH<Material> MaterialFactory::GetDefault()
{
    return MaterialFactory::Load("Default.bmat");
}
RH<Material> MaterialFactory::GetDefaultUnLighted()
{
    return MaterialFactory::Load("DefaultUnLighted.bmat");
}
RH<Material> MaterialFactory::GetGizmosUnLightedOverlay()
{
    return MaterialFactory::Load("GizmosUnLightedOverlay.bmat");
}
RH<Material> MaterialFactory::GetMissing()
{
    return MaterialFactory::Load("Missing.bmat");
}

RH<Material> MaterialFactory::GetUIText()
{
    return MaterialFactory::Load("UITextRenderer.bmat");
}
RH<Material> MaterialFactory::GetUIImage()
{
    return MaterialFactory::Load("UIImageRenderer.bmat");
}

RH<Material> MaterialFactory::Load(const String &matEnginePathStr)
{
    Path matEnginePath = Paths::GetEngineAssetsDir().Append("Materials").
                         Append(matEnginePathStr);
    MaterialFactory *mf = MaterialFactory::GetActive();
    if (!mf->m_cache.ContainsKey(matEnginePath))
    {
        mf->m_cache.Add(matEnginePath,
                        Resources::Load<Material>(matEnginePath));
    }
    return mf->m_cache.Get(matEnginePath);
}

MaterialFactory *MaterialFactory::GetActive()
{
    return Resources::GetActive()->GetMaterialFactory();
}
