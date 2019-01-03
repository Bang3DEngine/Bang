#include "Bang/Assets.h"

#include <sstream>
#include <unordered_map>
#include <utility>
#include <vector>

#include "Bang/Application.h"
#include "Bang/Array.tcc"
#include "Bang/Assert.h"
#include "Bang/Assets.tcc"
#include "Bang/CubeMapIBLGenerator.h"
#include "Bang/Debug.h"
#include "Bang/EventEmitter.h"
#include "Bang/EventEmitter.tcc"
#include "Bang/EventListener.tcc"
#include "Bang/Extensions.h"
#include "Bang/File.h"
#include "Bang/GUID.h"
#include "Bang/IEventsDestroy.h"
#include "Bang/MaterialFactory.h"
#include "Bang/MeshFactory.h"
#include "Bang/MetaFilesManager.h"
#include "Bang/Model.h"
#include "Bang/Paths.h"
#include "Bang/ShaderProgramFactory.h"
#include "Bang/TextureFactory.h"
#include "Bang/UMap.tcc"

using namespace Bang;

Assets::Assets()
{
}

void Assets::Init()
{
    m_meshFactory = CreateMeshFactory();
    m_textureFactory = CreateTextureFactory();
    m_materialFactory = new MaterialFactory();
    m_shaderProgramFactory = new ShaderProgramFactory();
}

void Assets::InitAfterGL()
{
    m_cubeMapIBLGenerator = new CubeMapIBLGenerator();
}

Assets::~Assets()
{
    Destroy();
}

AH<Asset> Assets::LoadFromExtension(const Path &path)
{
    AH<Asset> assetAH;
    if (path.HasExtension(Extensions::GetModelExtensions()))
    {
        assetAH.Set(Assets::Load<Model>(path).Get());
    }
    return assetAH;
}

void Assets::Import(Asset *asset)
{
    asset->Import_(asset->GetAssetFilepath());
}

Array<Asset *> Assets::GetAllAssets()
{
    Array<Asset *> result;
    Assets *assets = Assets::GetInstance();
    for (const auto &it : assets->m_assetsCache)
    {
        result.PushBack(it.second.asset);
    }
    return result;
}

void Assets::CreateAssetMetaAndImportFile(const Asset *asset,
                                          const Path &exportFilepath)
{
    File::Write(exportFilepath, "");
    Path metaFilePath = MetaFilesManager::GetMetaFilepath(exportFilepath);
    asset->ExportMetaToFile(metaFilePath);
    MetaFilesManager::RegisterMetaFilepath(metaFilePath);  // Once created
}

AH<Asset> Assets::Load_(std::function<Asset *()> creator, const Path &filepath)
{
    if (m_beingDestroyed || filepath.IsEmpty())
    {
        return AH<Asset>(nullptr);
    }

    if (!Assets::IsEmbeddedAsset(filepath) && !filepath.IsFile())
    {
        Debug_Warn("Filepath '" << filepath.GetAbsolute() << "' not found");
        return AH<Asset>(nullptr);
    }

    AH<Asset> assetAH;
    if (!Assets::IsEmbeddedAsset(filepath))
    {
        Asset *asset = GetCached_(filepath);
        assetAH.Set(asset);
        if (!asset)
        {
            // Create the asset
            asset = creator();

            GUID assetGUID = MetaFilesManager::GetGUID(filepath);
            if (!assetGUID.IsEmpty())
            {
                asset->SetGUID(assetGUID);
            }
            ASSERT(assetGUID != GUID::Empty());

            MetaFilesManager::RegisterFilepathGUID(filepath, assetGUID);
        }

        if (asset)
        {
            assetAH.Set(asset);
            Assets::Import(asset);  // Actually import the asset
        }
    }
    else
    {
        AH<Asset> parentAssetAH =
            Assets::LoadFromExtension(filepath.GetDirectory());
        if (parentAssetAH)
        {
            String embeddedAssetName = filepath.GetNameExt();
            assetAH.Set(
                parentAssetAH.Get()->GetEmbeddedAsset(embeddedAssetName));
        }
    }
    return assetAH;
}

AH<Asset> Assets::Load_(std::function<Asset *()> creator, const GUID &guid)
{
    if (guid.IsEmpty())
    {
        return AH<Asset>(nullptr);
    }

    AH<Asset> assetAH(GetCached_(guid));
    if (!assetAH)
    {
        if (!Assets::IsEmbeddedAsset(guid))
        {
            const Path assetPath = MetaFilesManager::GetFilepath(guid);
            if (assetPath.IsFile())
            {
                assetAH.Set(Load_(creator, assetPath).Get());
            }
        }
        else
        {
            const GUID parentGUID = guid.WithoutEmbeddedAssetGUID();
            const Path parentPath = MetaFilesManager::GetFilepath(parentGUID);
            if (parentPath.IsFile())
            {
                if (AH<Asset> parentAssetAH =
                        Assets::LoadFromExtension(parentPath))
                {
                    assetAH.Set(parentAssetAH.Get()->GetEmbeddedAsset(guid));
                }
            }
        }
    }
    return assetAH;
}

Asset *Assets::GetCached_(const GUID &guid) const
{
    if (m_assetsCache.ContainsKey(guid))
    {
        return m_assetsCache.Get(guid).asset;
    }
    /*
    else
    {
        if (Assets::IsEmbeddedAsset(guid))
        {
            GUID parentGUID = guid.WithoutEmbeddedAssetGUID();
            if (Asset *parentAsset = GetCached_(parentGUID))
            {
                return
    parentAsset->GetEmbeddedAsset(guid.GetEmbeddedAssetGUID());
            }
        }
    }*/
    return nullptr;
}

Asset *Assets::GetCached_(const Path &path) const
{
    GUID guid = MetaFilesManager::GetGUID(path);
    return GetCached_(guid);
}

bool Assets::Contains_(Asset *asset) const
{
    return asset && GetCached_(asset->GetGUID());
}

void Assets::Add(Asset *asset)
{
    const GUID &guid = asset->GetGUID();
    ASSERT(asset != nullptr);
    ASSERT(!guid.IsEmpty());

    Assets *assets = Assets::GetInstance();
    ASSERT(assets);
    ASSERT(!assets->GetCached_(guid));

    AssetEntry assetEntry;
    assetEntry.asset = asset;
    assetEntry.usageCount = 0;
    assets->m_assetsCache.Add(guid, assetEntry);
}

bool Assets::IsEmbeddedAsset(const GUID &guid)
{
    return (guid.GetEmbeddedAssetGUID() != GUID::EmptyGUID());
}

bool Assets::IsEmbeddedAsset(const Path &assetPath)
{
    return assetPath.GetDirectory().IsFile();
}

void Assets::Remove(const GUID &guid)
{
    Assets *assets = Assets::GetInstance();
    ASSERT(assets);

    auto it = assets->m_assetsCache.Find(guid);
    ASSERT(it != assets->m_assetsCache.End());

    const AssetEntry &assetEntry = it->second;
    ASSERT(assetEntry.asset != nullptr);
    ASSERT(assetEntry.usageCount == 0);

    Asset *asset = assetEntry.asset;
    if (asset)
    {
        if (EventEmitter<IEventsDestroy> *destroyable =
                DCAST<EventEmitter<IEventsDestroy> *>(asset))
        {
            destroyable->EventEmitter<IEventsDestroy>::PropagateToListeners(
                &IEventsDestroy::OnDestroyed, destroyable);
        }
    }

    assets->m_assetsCache.Remove(it);

    if (asset)
    {
        delete asset;
    }
}

Array<Path> Assets::GetLookUpPaths() const
{
    return {Paths::GetProjectAssetsDir(), Paths::GetEngineAssetsDir()};
}

void Assets::RegisterAssetUsage(Asset *asset)
{
    const GUID &guid = asset->GetGUID();
    ASSERT(!guid.IsEmpty());

    Assets *assets = Assets::GetInstance();
    if (!assets->GetCached_(guid))
    {
        Assets::Add(asset);
    }
    ++assets->m_assetsCache.Get(guid).usageCount;
}

void Assets::UnRegisterAssetUsage(Asset *asset)
{
    if (Assets *assets = Assets::GetInstance())
    {
        const GUID &guid = asset->GetGUID();
        ASSERT(!guid.IsEmpty());

        ASSERT(assets->GetCached_(guid));
        uint *assetsUsage = &(assets->m_assetsCache.Get(guid).usageCount);
        ASSERT(*assetsUsage >= 1);
        --(*assetsUsage);

        if (*assetsUsage == 0)
        {
            Assets::Remove(guid);
        }
    }
}

Path Assets::GetAssetPath(const Asset *asset)
{
    Path assetPath;
    if (asset)
    {
        assetPath = MetaFilesManager::GetFilepath(asset->GetGUID());
    }
    return assetPath;
}

MeshFactory *Assets::GetMeshFactory() const
{
    return m_meshFactory;
}

TextureFactory *Assets::GetTextureFactory() const
{
    return m_textureFactory;
}

MaterialFactory *Assets::GetMaterialFactory() const
{
    return m_materialFactory;
}

CubeMapIBLGenerator *Assets::GetCubeMapIBLGenerator() const
{
    return m_cubeMapIBLGenerator;
}

ShaderProgramFactory *Assets::GetShaderProgramFactory() const
{
    return m_shaderProgramFactory;
}

void Assets::Destroy()
{
    m_beingDestroyed = true;

#define B_DESTROY_AND_NULL(p) \
    if (p)                    \
    {                         \
        delete p;             \
        p = nullptr;          \
    }
    B_DESTROY_AND_NULL(m_meshFactory);
    B_DESTROY_AND_NULL(m_textureFactory);
    B_DESTROY_AND_NULL(m_materialFactory);
    B_DESTROY_AND_NULL(m_cubeMapIBLGenerator);
    B_DESTROY_AND_NULL(m_shaderProgramFactory);
#undef B_DESTROY_AND_NULL
}

Assets *Assets::GetInstance()
{
    Application *app = Application::GetInstance();
    return app ? app->GetAssets() : nullptr;
}

MeshFactory *Assets::CreateMeshFactory() const
{
    return new MeshFactory();
}

TextureFactory *Assets::CreateTextureFactory() const
{
    return new TextureFactory();
}
