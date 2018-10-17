#ifndef MATERIALFACTORY_H
#define MATERIALFACTORY_H

#include "Bang/BangDefines.h"
#include "Bang/RenderPass.h"
#include "Bang/ResourceHandle.h"
#include "Bang/String.h"
#include "Bang/UMap.h"

namespace Bang
{
class Material;
class PhysicsMaterial;

class MaterialFactory
{
public:
    static RH<Material> GetDefault(RenderPass renderPass = RenderPass::SCENE);
    static RH<Material> GetDefaultUnLighted();
    static RH<Material> GetGizmosUnLightedOverlay();
    static RH<Material> GetParticlesAdditive();
    static RH<Material> GetParticlesMesh();
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
    static RH<PhysicsMaterial> LoadPhysicsMaterial(
        const String &phMatEnginePath);

    static MaterialFactory *GetActive();

    friend class Resources;
};
}

#endif  // MATERIALFACTORY_H
