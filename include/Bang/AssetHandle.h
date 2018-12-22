#ifndef ASSETHANDLE_H
#define ASSETHANDLE_H

#include "Bang/BangDefines.h"

namespace Bang
{
class Asset;

void OnAssetSet(Asset *asset);
void OnAssetUnSet(Asset *asset);

class AssetHandleBase
{
protected:
    AssetHandleBase() = default;
    virtual ~AssetHandleBase();

    bool operator==(const Asset *rhs) const;
    bool operator==(const AssetHandleBase &rhs) const;
    bool operator!=(const AssetHandleBase &rhs) const;
    bool operator<(const AssetHandleBase &rhs) const;
    operator bool() const;

    Asset *Get() const;

    void Set(Asset *asset);

private:
    Asset *p_asset = nullptr;
};

template <class AssetClass>
class AssetHandle : public AssetHandleBase
{
public:
    AssetHandle() = default;

    explicit AssetHandle(AssetClass *asset) : AssetHandleBase()
    {
        Set(asset);
    }
    AssetHandle(AssetHandle<AssetClass> &&rhs)
    {
        *this = rhs;
    }
    AssetHandle(const AssetHandle<AssetClass> &rhs)
    {
        *this = rhs;
    }

    void Set(AssetClass *asset)
    {
        AssetHandleBase::Set(SCAST<Asset *>(asset));
    }

    AssetClass *Get() const
    {
        return SCAST<AssetClass *>(AssetHandleBase::Get());
    }

    bool operator==(const AssetClass *rhs) const
    {
        return AssetHandleBase::operator=(rhs);
    }
    bool operator==(const AssetHandleBase &rhs) const
    {
        return AssetHandleBase::operator==(rhs);
    }
    bool operator!=(const AssetHandleBase &rhs) const
    {
        return AssetHandleBase::operator!=(rhs);
    }
    bool operator<(const AssetHandleBase &rhs) const
    {
        return AssetHandleBase::operator<(rhs);
    }
    operator bool() const
    {
        return AssetHandleBase::operator bool();
    }

    AssetHandle<AssetClass> &operator=(const AssetHandle<AssetClass> &rhs)
    {
        if (&rhs != this)
        {
            Set(rhs.Get());
        }
        return *this;
    }

    AssetHandle &operator=(AssetHandle<AssetClass> &&rhs)
    {
        if (&rhs != this)
        {
            Set(rhs.Get());
            rhs.Set(nullptr);
        }
        return *this;
    }

private:
    friend class Assets;
};

template <class T>
using AH = AssetHandle<T>;
}

#endif  // ASSETHANDLE_H
