#ifndef ASSET_H
#define ASSET_H

#include <functional>
#include <vector>

#include "Bang/Array.h"
#include "Bang/Array.tcc"
#include "Bang/AssetHandle.h"
#include "Bang/BangDefines.h"
#include "Bang/EventEmitter.h"
#include "Bang/EventListener.tcc"
#include "Bang/GUID.h"
#include "Bang/IEventsAsset.h"
#include "Bang/IEventsDestroy.h"
#include "Bang/Map.h"
#include "Bang/MetaNode.h"
#include "Bang/Path.h"
#include "Bang/Serializable.h"
#include "Bang/String.h"

namespace Bang
{
class IEventsAsset;

class Asset : public Serializable,
              public EventEmitter<IEventsDestroy>,
              public EventEmitter<IEventsAsset>
{
public:
    void RemoveEmbeddedAsset(Asset *asset);
    void RemoveEmbeddedAsset(const String &embeddedAssetName);
    void PropagateAssetChanged();

    Path GetAssetFilepath() const;
    Asset *GetParentAsset() const;
    const Array<AH<Asset>> &GetEmbeddedAssets() const;
    Asset *GetEmbeddedAsset(const GUID &embeddedResGUID) const;
    Asset *GetEmbeddedAsset(const String &embeddedAssetName) const;
    Asset *GetEmbeddedAsset(GUID::GUIDType embeddedResGUID) const;
    String GetEmbeddedAssetName(GUID::GUIDType embeddedResGUID) const;

protected:
    Asset();
    virtual ~Asset() override;

    void ClearEmbeddedAssets();

    // Serializable
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;

    // Asset
    virtual void Import(const Path &assetFilepath);
    void Import_(const Path &assetFilepath);

private:
    // Embedded asset related variables
    AH<Asset> p_parentAsset;
    Array<AH<Asset>> m_embeddedAssets;
    Map<String, Asset *> m_nameToEmbeddedAsset;
    Map<Asset *, String> m_embeddedAssetToName;
    Map<GUID, Asset *> m_GUIDToEmbeddedAsset;
    mutable GUID::GUIDType m_nextNewEmbeddedAssetGUID = 1;

    void AddEmbeddedAsset(const String &embeddedAssetName,
                          Asset *embeddedAsset);
    GUID::GUIDType GetNewEmbeddedAssetGUID() const;

    void SetParentAsset(Asset *parentAsset);

    friend class Assets;
};

#define ASSET_ABSTRACT(CLASSNAME) \
    SERIALIZABLE(CLASSNAME)       \
    friend class Assets;

#define ASSET(CLASSNAME)      \
    ASSET_ABSTRACT(CLASSNAME) \
    ICLONEABLE(CLASSNAME)
}

#endif  // ASSET_H
