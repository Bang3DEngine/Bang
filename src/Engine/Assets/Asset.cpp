#include "Bang/Asset.h"

#include <utility>

#include "Bang/Assets.h"
#include "Bang/EventEmitter.tcc"
#include "Bang/IEventsAsset.h"
#include "Bang/Map.tcc"
#include "Bang/MetaFilesManager.h"
#include "Bang/MetaNode.h"

using namespace Bang;

Asset::Asset()
{
}

Asset::~Asset()
{
    EventEmitter<IEventsDestroy>::PropagateToListeners(
        &IEventsDestroy::OnDestroyed, this);

    if (GetParentAsset())
    {
        GetParentAsset()->RemoveEmbeddedAsset(this);
    }

#ifdef DEBUG
    bool stillOnAssets = Assets::Contains(GetGUID());
    ASSERT(!stillOnAssets);
#endif
}

void Asset::AddEmbeddedAsset(const String &embeddedAssetName,
                             Asset *embeddedAsset)
{
    if (!m_embeddedAssetToName.ContainsKey(embeddedAsset))
    {
        m_embeddedAssetToName.Add(embeddedAsset);
        m_nameToEmbeddedAsset.Add(embeddedAssetName, embeddedAsset);
        m_embeddedAssetToName.Add(embeddedAsset, embeddedAssetName);
        m_GUIDToEmbeddedAsset.Add(embeddedAsset->GetGUID(), embeddedAsset);

        AH<Asset> prevParentAsset(embeddedAsset->GetParentAsset());
        if (prevParentAsset)
        {
            prevParentAsset.Get()->RemoveEmbeddedAsset(embeddedAsset);
        }
        embeddedAsset->SetParentAsset(this);
    }
}
void Asset::RemoveEmbeddedAsset(Asset *embeddedAsset)
{
    m_embeddedAssetToName.Remove(embeddedAsset);
    m_nameToEmbeddedAsset.RemoveValues(embeddedAsset);
    m_GUIDToEmbeddedAsset.RemoveValues(embeddedAsset);
    embeddedAsset->SetParentAsset(nullptr);

    for (const AH<Asset> &assetAH : m_embeddedAssets)
    {
        if (assetAH.Get() == embeddedAsset)
        {
            m_embeddedAssets.Remove(assetAH);
            break;
        }
    }
}
void Asset::RemoveEmbeddedAsset(const String &embeddedAssetName)
{
    if (m_nameToEmbeddedAsset.ContainsKey(embeddedAssetName))
    {
        Asset *embeddedAsset = m_nameToEmbeddedAsset.Get(embeddedAssetName);
        RemoveEmbeddedAsset(embeddedAsset);
    }
}

void Asset::SetParentAsset(Asset *parentAsset)
{
    p_parentAsset.Set(parentAsset);
}

Asset *Asset::GetParentAsset() const
{
    return p_parentAsset.Get();
}

const Array<AH<Asset>> &Asset::GetEmbeddedAssets() const
{
    return m_embeddedAssets;
}

void Asset::PropagateAssetChanged()
{
    if (EventEmitter<IEventsAsset>::IsEmittingEvents())
    {
        EventEmitter<IEventsAsset>::PropagateToListeners(
            &IEventsAsset::OnAssetChanged, this);
        if (GetParentAsset())
        {
            GetParentAsset()->PropagateAssetChanged();
        }
    }
}

// Asset
Path Asset::GetAssetFilepath() const
{
    return MetaFilesManager::GetFilepath(GetGUID());
}

Asset *Asset::GetEmbeddedAsset(const GUID &embeddedResGUID) const
{
    if (m_GUIDToEmbeddedAsset.ContainsKey(embeddedResGUID))
    {
        Asset *embeddedAsset = m_GUIDToEmbeddedAsset.Get(embeddedResGUID);
        return embeddedAsset;
    }
    return nullptr;
}

Asset *Asset::GetEmbeddedAsset(const String &embeddedAssetName) const
{
    if (m_nameToEmbeddedAsset.ContainsKey(embeddedAssetName))
    {
        Asset *embeddedAsset = m_nameToEmbeddedAsset.Get(embeddedAssetName);
        return embeddedAsset;
    }
    return nullptr;
}

Asset *Asset::GetEmbeddedAsset(GUID::GUIDType embeddedResGUID) const
{
    GUID embeddedAssetFullGUID =
        GetGUID().WithEmbeddedAssetGUID(embeddedResGUID);
    return GetEmbeddedAsset(embeddedAssetFullGUID);
}

String Asset::GetEmbeddedAssetName(GUID::GUIDType embeddedResGUID) const
{
    Asset *embeddedAsset = GetEmbeddedAsset(embeddedResGUID);
    if (embeddedAsset)
    {
        if (m_embeddedAssetToName.ContainsKey(embeddedAsset))
        {
            return m_embeddedAssetToName.Get(embeddedAsset);
        }
    }
    return "";
}

void Asset::ImportMeta(const MetaNode &metaNode)
{
    Serializable::ImportMeta(metaNode);

    for (const auto &pair : m_nameToEmbeddedAsset)
    {
        if (Asset *embeddedRes = pair.second)
        {
            const String &embeddedResName = pair.first;
            if (const MetaNode *embeddedResMetaNode =
                    metaNode.GetChild(embeddedResName))
            {
                embeddedRes->ImportMeta(*embeddedResMetaNode);
            }
        }
    }
}

void Asset::ExportMeta(MetaNode *metaNode) const
{
    Serializable::ExportMeta(metaNode);

    for (const auto &pair : m_nameToEmbeddedAsset)
    {
        if (Asset *embeddedRes = pair.second)
        {
            const String &embeddedResName = pair.first;
            MetaNode embeddedResMeta = embeddedRes->GetMeta();
            embeddedResMeta.SetName(embeddedResName);
            metaNode->AddChild(embeddedResMeta, "Children");
        }
    }
}

void Asset::Import(const Path &assetFilepath)
{
    BANG_UNUSED(assetFilepath);
}

void Asset::Import_(const Path &assetFilepath)
{
    Import(assetFilepath);
    Path importFilepath = MetaFilesManager::GetMetaFilepath(assetFilepath);
    ImportMetaFromFile(importFilepath);  // Import Meta then

    EventEmitter<IEventsAsset>::PropagateToListeners(&IEventsAsset::OnImported,
                                                     this);
}

void Asset::ClearEmbeddedAssets()
{
    while (!m_embeddedAssets.IsEmpty())
    {
        Asset *embeddedAsset = m_embeddedAssets.Back().Get();
        RemoveEmbeddedAsset(embeddedAsset);
    }

    m_nameToEmbeddedAsset.Clear();
    m_embeddedAssetToName.Clear();
    m_GUIDToEmbeddedAsset.Clear();
    m_nextNewEmbeddedAssetGUID = 1;
}

GUID::GUIDType Asset::GetNewEmbeddedAssetGUID() const
{
    auto nextNewGUID = m_nextNewEmbeddedAssetGUID;
    ++m_nextNewEmbeddedAssetGUID;
    return nextNewGUID;
}
