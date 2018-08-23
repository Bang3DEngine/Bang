#ifndef MATERIALFACTORY_H
#define MATERIALFACTORY_H

#include "Bang/UMap.h"
#include "Bang/Material.h"
#include "Bang/ResourceHandle.h"
#include "Bang/PhysicsMaterial.h"

NAMESPACE_BANG_BEGIN

class MaterialFactory
{
public:
    static RH<Material> GetDefault(RenderPass renderPass = RenderPass::SCENE);
    static RH<Material> GetDefaultAnimated();
    static RH<Material> GetDefaultUnLighted();
    static RH<Material> GetGizmosUnLightedOverlay();
    static RH<Material> GetWater();

    static RH<PhysicsMaterial> GetDefaultPhysicsMaterial();

    static RH<Material> GetMissing();

    static RH<Material> GetUIText();
    static RH<Material> GetUIImage();

private:
    UMap<String, RH<Material>> m_cacheMaterials;
    UMap<String, RH<PhysicsMaterial>> m_cachePhysicsMaterials;

    MaterialFactory() = default;
    static RH<Material> LoadMaterial(const String &matEnginePath);
    static RH<PhysicsMaterial> LoadPhysicsMaterial(const String &phMatEnginePath);

    static MaterialFactory* GetActive();

    friend class Resources;
};

NAMESPACE_BANG_END

#endif // MATERIALFACTORY_H
