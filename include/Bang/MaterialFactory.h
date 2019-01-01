#ifndef MATERIALFACTORY_H
#define MATERIALFACTORY_H

#include "Bang/UMap.h"

#include "Bang/AssetHandle.h"
#include "Bang/BangDefines.h"
#include "Bang/Path.h"
#include "Bang/RenderPass.h"
#include "Bang/String.h"

namespace Bang
{
class Material;
class PhysicsMaterial;

class MaterialFactory
{
public:
    static AH<Material> GetDefault(
        RenderPass renderPass = RenderPass::SCENE_OPAQUE);
    static AH<Material> GetGizmosUnLightedOverlay();
    static AH<Material> GetParticlesAdditive();
    static AH<Material> GetParticlesMesh();
    static AH<Material> GetWater();

    static AH<PhysicsMaterial> GetDefaultPhysicsMaterial();

    static AH<Material> GetMissing();

    static AH<Material> GetUIText();
    static AH<Material> GetUIImage();

private:
    UMap<Path, AH<Material>> m_cacheMaterials;
    UMap<Path, AH<PhysicsMaterial>> m_cachePhysicsMaterials;

    MaterialFactory() = default;
    static AH<Material> LoadMaterial(const String &matEnginePath);
    static AH<PhysicsMaterial> LoadPhysicsMaterial(
        const String &phMatEnginePath);

    static MaterialFactory *GetActive();

    friend class Assets;
};
}  // namespace Bang

#endif  // MATERIALFACTORY_H
