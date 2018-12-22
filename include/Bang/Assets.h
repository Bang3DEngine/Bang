#ifndef ASSETS_H
#define ASSETS_H

#include <functional>

#include "Bang/Array.h"
#include "Bang/Asset.h"
#include "Bang/AssetHandle.h"
#include "Bang/BangDefines.h"
#include "Bang/IToString.h"
#include "Bang/Path.h"
#include "Bang/String.h"
#include "Bang/UMap.h"
#include "Bang/USet.h"

namespace Bang
{
class GUID;
}  // namespace Bang

namespace Bang
{
class CubeMapIBLGenerator;
class MaterialFactory;
class MeshFactory;
class ShaderProgramFactory;
class TextureFactory;

class Assets
{
private:
    struct AssetEntry : public IToString
    {
        Asset *asset = nullptr;
        uint usageCount = 0;  // Number of AH's using this asset entry
        String ToString() const override
        {
            return "AE(" + String(asset) + ", " + String(usageCount) + ")";
        }
    };

public:
    Assets();
    virtual ~Assets();

    static void Add(Asset *asset);

    template <class AssetClass = Asset, class... Args>
    static AH<AssetClass> Create(const Args &... args);
    template <class AssetClass = Asset, class... Args>
    static AH<AssetClass> Create(const GUID &guid, const Args &... args);
    template <class AssetClass = Asset, class... Args>
    static AH<AssetClass> CreateEmbeddedAsset(Asset *parentAsset,
                                              const String &embeddedAssetName,
                                              const Args &... args);
    static void CreateAssetMetaAndImportFile(const Asset *asset,
                                             const Path &exportFilepath);

    template <class AssetClass = Asset>
    static AH<AssetClass> Load(const Path &filepath);
    template <class AssetClass = Asset>
    static AH<AssetClass> Load(const GUID &guid);

    static AH<Asset> LoadFromExtension(const Path &filepath);

    static void Import(Asset *asset);

    template <class AssetClass = Asset>
    static AH<AssetClass> Clone(const AssetClass *src);

    static void RegisterAssetUsage(Asset *asset);
    static void UnRegisterAssetUsage(Asset *asset);

    static void Remove(const GUID &guid);

    static bool IsEmbeddedAsset(const GUID &guid);
    static bool IsEmbeddedAsset(const Path &path);

    template <class AssetClass = Asset>
    static bool Contains(const GUID &guid);

    template <class AssetClass = Asset>
    static AssetClass *GetCached(const GUID &guid);
    template <class AssetClass = Asset>
    static AssetClass *GetCached(const Path &path);

    static Path GetAssetPath(const Asset *asset);

    template <class AssetClass = Asset>
    static Array<AssetClass *> GetAll();
    static Array<Asset *> GetAllAssets();

    MeshFactory *GetMeshFactory() const;
    TextureFactory *GetTextureFactory() const;
    MaterialFactory *GetMaterialFactory() const;
    CubeMapIBLGenerator *GetCubeMapIBLGenerator() const;
    ShaderProgramFactory *GetShaderProgramFactory() const;

    virtual void Init();
    virtual void InitAfterGL();
    virtual Array<Path> GetLookUpPaths() const;

    void Destroy();

    static Assets *GetInstance();

private:
    bool m_beingDestroyed = false;
    UMap<GUID, AssetEntry> m_assetsCache;

    MeshFactory *m_meshFactory = nullptr;
    TextureFactory *m_textureFactory = nullptr;
    MaterialFactory *m_materialFactory = nullptr;
    CubeMapIBLGenerator *m_cubeMapIBLGenerator = nullptr;
    ShaderProgramFactory *m_shaderProgramFactory = nullptr;

    virtual MeshFactory *CreateMeshFactory() const;
    virtual TextureFactory *CreateTextureFactory() const;

    template <class AssetClass, class... Args>
    static AssetClass *Create_(const Args &... args);
    template <class AssetClass, class... Args>
    static AssetClass *Create_(const GUID &guid, const Args &... args);

    AH<Asset> Load_(std::function<Asset *()> creator, const Path &path);
    AH<Asset> Load_(std::function<Asset *()> creator, const GUID &guid);

    Asset *GetCached_(const GUID &guid) const;
    Asset *GetCached_(const Path &path) const;
    bool Contains_(Asset *asset) const;

    friend class Window;
    friend class GUIDManager;
    friend class IAssetHandle;
};
}

#include "Bang/Assets.tcc"

#endif  // ASSETS_H
