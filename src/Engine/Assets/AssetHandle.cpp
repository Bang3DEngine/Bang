#include "Bang/AssetHandle.h"

#include "Bang/Assets.h"

namespace Bang
{
void OnAssetSet(Asset *asset)
{
    Assets::RegisterAssetUsage(asset);
}

void OnAssetUnSet(Asset *asset)
{
    Assets::UnRegisterAssetUsage(asset);
}

Bang::AssetHandleBase::operator bool() const
{
    return (Get() != nullptr);
}

bool AssetHandleBase::operator==(const AssetHandleBase &rhs) const
{
    return Get() == rhs.Get();
}

AssetHandleBase::~AssetHandleBase()
{
    Set(nullptr);
}

bool AssetHandleBase::operator==(const Asset *rhs) const
{
    return Get() == rhs;
}

bool AssetHandleBase::operator!=(const AssetHandleBase &rhs) const
{
    return !(*this == rhs);
}

bool AssetHandleBase::operator<(const AssetHandleBase &rhs) const
{
    return Get() < rhs.Get();
}

Asset *AssetHandleBase::Get() const
{
    return p_asset;
}

void AssetHandleBase::Set(Asset *asset)
{
    if (Get() != asset)
    {
        if (Get())
        {
            // Must be done in two steps, so that we avoid unset loops
            Asset *prevAsset = p_asset;
            p_asset = nullptr;
            OnAssetUnSet(prevAsset);
        }

        p_asset = asset;
        if (Get())
        {
            OnAssetSet(Get());
        }
    }
}
}
