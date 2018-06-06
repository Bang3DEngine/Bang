#ifndef MATERIALFACTORY_H
#define MATERIALFACTORY_H

#include "Bang/UMap.h"
#include "Bang/Material.h"
#include "Bang/ResourceHandle.h"

NAMESPACE_BANG_BEGIN

class MaterialFactory
{
public:
    static RH<Material> GetDefault(RenderPass renderPass = RenderPass::SCENE);
    static RH<Material> GetDefaultUnLighted();
    static RH<Material> GetGizmosUnLightedOverlay();

    static RH<Material> GetMissing();

    static RH<Material> GetUIText();
    static RH<Material> GetUIImage();

private:
    UMap<String, RH<Material>> m_cache;

    MaterialFactory() = default;
    static RH<Material> Load(const String &matEnginePath);

    static MaterialFactory* GetActive();

    friend class Resources;
};

NAMESPACE_BANG_END

#endif // MATERIALFACTORY_H
