#pragma once

#include "Bang/Assets.h"
#include "Bang/GUIDManager.h"
#include "Bang/StreamOperators.h"

#include "Bang/Debug.h"

namespace Bang
{
template <class AssetClass>
AH<AssetClass> Assets::Load(const Path &filepath)
{
    Assets *assets = Assets::GetInstance();
    auto creator = []() -> Asset * {
        return SCAST<Asset *>(Assets::Create_<AssetClass>());
    };

    AH<AssetClass> resultAH;
    {
        AH<Asset> assetAH = assets->Load_(creator, filepath);
        resultAH.Set(SCAST<AssetClass *>(assetAH.Get()));
        if (resultAH && !Assets::IsEmbeddedAsset(filepath))
        {
            ASSERT_MSG(DCAST<AssetClass *>(resultAH.Get()),
                       "Asset " << filepath
                                << " being loaded "
                                   "as two different types of assets. "
                                   "This is forbidden");
        }
    }
    return resultAH;
}

template <class AssetClass>
AH<AssetClass> Assets::Load(const GUID &guid)
{
    Assets *assets = Assets::GetInstance();
    auto creator = []() -> Asset * {
        return SCAST<Asset *>(Assets::Create_<AssetClass>());
    };

    AH<AssetClass> resultAH;
    {
        AH<Asset> assetAH = assets->Load_(creator, guid);
        if (assetAH)
        {
            if (AssetClass *asset = DCAST<AssetClass *>(assetAH.Get()))
            {
                resultAH = AH<AssetClass>(asset);
            }
            else
            {
                ASSERT_MSG(asset,
                           "Asset " << guid
                                    << " being loaded "
                                       "as two different types of assets. "
                                       "This is forbidden");
            }
        }
    }
    return resultAH;
}

template <class AssetClass, class... Args>
AH<AssetClass> Assets::Create(const Args &... args)
{
    return AH<AssetClass>(Assets::Create_<AssetClass, Args...>(args...));
}
template <class AssetClass, class... Args>
AH<AssetClass> Assets::Create(const GUID &guid, const Args &... args)
{
    return AH<AssetClass>(Assets::Create_<AssetClass, Args...>(guid, args...));
}

template <class AssetClass, class... Args>
AssetClass *Assets::Create_(const Args &... args)
{
    return Create_<AssetClass, Args...>(GUIDManager::GetNewGUID(), args...);
}

template <class AssetClass, class... Args>
AssetClass *Assets::Create_(const GUID &guid, const Args &... args)
{
    AssetClass *asset = new AssetClass(args...);
    asset->SetGUID(guid);
    return asset;
}

template <class AssetClass, class... Args>
AH<AssetClass> Assets::CreateEmbeddedAsset(Asset *parentAsset,
                                           const String &embeddedAssetName,
                                           const Args &... args)
{
    GUID::GUIDType newAssetEmbeddedAssetGUID =
        parentAsset->GetNewEmbeddedAssetGUID();
    GUID newAssetEmbeddedAssetFullGUID;
    GUIDManager::CreateEmbeddedFileGUID(parentAsset->GetGUID(),
                                        newAssetEmbeddedAssetGUID,
                                        &newAssetEmbeddedAssetFullGUID);
    AH<AssetClass> embeddedAsset = Assets::Create<AssetClass, Args...>(
        newAssetEmbeddedAssetFullGUID, args...);

    parentAsset->AddEmbeddedAsset(embeddedAssetName, embeddedAsset.Get());

    return embeddedAsset;
}

template <class AssetClass>
Array<AssetClass *> Assets::GetAll()
{
    Array<AssetClass *> result;
    Array<Asset *> assets = Assets::GetAllAssets();
    for (Asset *asset : assets)
    {
        if (asset)
        {
            if (AssetClass *ac = DCAST<AssetClass *>(asset))
            {
                result.PushBack(ac);
            }
        }
    }
    return result;
}

template <class AssetClass>
bool Assets::Contains(const GUID &guid)
{
    return Assets::GetInstance()->GetCached_(guid) != nullptr;
}

template <class AssetClass>
AssetClass *Assets::GetCached(const GUID &guid)
{
    Asset *asset = Assets::GetInstance()->GetCached_(guid);
    return SCAST<AssetClass *>(asset);
}
template <class AssetClass>
AssetClass *Assets::GetCached(const Path &path)
{
    Asset *asset = Assets::GetInstance()->GetCached_(path);
    return SCAST<AssetClass *>(asset);
}

template <class AssetClass>
AH<AssetClass> Assets::Clone(const AssetClass *src)
{
    AH<AssetClass> ah;
    if (src)
    {
        ah = Assets::Create<AssetClass>();
        src->CloneInto(ah.Get(), false);
    }
    return ah;
}
}  // namespace Bang
