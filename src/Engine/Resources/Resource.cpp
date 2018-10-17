#include "Bang/Resource.h"

#include <utility>

#include "Bang/EventEmitter.tcc"
#include "Bang/IEventsResource.h"
#include "Bang/Map.tcc"
#include "Bang/MetaFilesManager.h"
#include "Bang/MetaNode.h"
#include "Bang/Resources.h"

using namespace Bang;

// IResource
Resource::Resource()
{
}

Resource::~Resource()
{
    if(GetParentResource())
    {
        GetParentResource()->RemoveEmbeddedResource(this);
    }

#ifdef DEBUG
    bool stillOnResources = Resources::Contains(GetGUID());
    ASSERT(!stillOnResources);
#endif
}

void Resource::AddEmbeddedResource(const String &embeddedResourceName,
                                   Resource *embeddedResource)
{
    if(!m_embeddedResourceToName.ContainsKey(embeddedResource))
    {
        m_embeddedResourceToName.Add(embeddedResource);
        m_nameToEmbeddedResource.Add(embeddedResourceName, embeddedResource);
        m_embeddedResourceToName.Add(embeddedResource, embeddedResourceName);
        m_GUIDToEmbeddedResource.Add(embeddedResource->GetGUID(),
                                     embeddedResource);

        RH<Resource> prevParentResource(embeddedResource->GetParentResource());
        if(prevParentResource)
        {
            prevParentResource.Get()->RemoveEmbeddedResource(embeddedResource);
        }
        embeddedResource->SetParentResource(this);
    }
}
void Resource::RemoveEmbeddedResource(Resource *embeddedResource)
{
    m_embeddedResourceToName.Remove(embeddedResource);
    m_nameToEmbeddedResource.RemoveValues(embeddedResource);
    m_GUIDToEmbeddedResource.RemoveValues(embeddedResource);
    embeddedResource->SetParentResource(nullptr);

    for(const RH<Resource> &res : m_embeddedResources)
    {
        if(res.Get() == embeddedResource)
        {
            m_embeddedResources.Remove(res);
            break;
        }
    }
}
void Resource::RemoveEmbeddedResource(const String &embeddedResourceName)
{
    if(m_nameToEmbeddedResource.ContainsKey(embeddedResourceName))
    {
        Resource *embeddedResource =
            m_nameToEmbeddedResource.Get(embeddedResourceName);
        RemoveEmbeddedResource(embeddedResource);
    }
}

void Resource::SetParentResource(Resource *parentResource)
{
    p_parentResource.Set(parentResource);
}

Resource *Resource::GetParentResource() const
{
    return p_parentResource.Get();
}

const Array<RH<Resource>> &Resource::GetEmbeddedResources() const
{
    return m_embeddedResources;
}

void Resource::PropagateResourceChanged()
{
    if(EventEmitter<IEventsResource>::IsEmittingEvents())
    {
        EventEmitter<IEventsResource>::PropagateToListeners(
            &IEventsResource::OnResourceChanged, this);
        if(GetParentResource())
        {
            GetParentResource()->PropagateResourceChanged();
        }
    }
}

// Resource
Path Resource::GetResourceFilepath() const
{
    return MetaFilesManager::GetFilepath(GetGUID());
}

Resource *Resource::GetEmbeddedResource(const GUID &embeddedResGUID) const
{
    if(m_GUIDToEmbeddedResource.ContainsKey(embeddedResGUID))
    {
        Resource *embeddedResource =
            m_GUIDToEmbeddedResource.Get(embeddedResGUID);
        return embeddedResource;
    }
    return nullptr;
}

Resource *Resource::GetEmbeddedResource(
    const String &embeddedResourceName) const
{
    if(m_nameToEmbeddedResource.ContainsKey(embeddedResourceName))
    {
        Resource *embeddedResource =
            m_nameToEmbeddedResource.Get(embeddedResourceName);
        return embeddedResource;
    }
    return nullptr;
}

Resource *Resource::GetEmbeddedResource(GUID::GUIDType embeddedResGUID) const
{
    GUID embeddedResourceFullGUID =
        GetGUID().WithEmbeddedResourceGUID(embeddedResGUID);
    return GetEmbeddedResource(embeddedResourceFullGUID);
}

String Resource::GetEmbeddedResourceName(GUID::GUIDType embeddedResGUID) const
{
    Resource *embeddedResource = GetEmbeddedResource(embeddedResGUID);
    if(embeddedResource)
    {
        if(m_embeddedResourceToName.ContainsKey(embeddedResource))
        {
            return m_embeddedResourceToName.Get(embeddedResource);
        }
    }
    return "";
}

void Resource::ImportMeta(const MetaNode &metaNode)
{
    Serializable::ImportMeta(metaNode);

    for(const auto &pair : m_nameToEmbeddedResource)
    {
        if(Resource *embeddedRes = pair.second)
        {
            const String &embeddedResName = pair.first;
            if(const MetaNode *embeddedResMetaNode =
                   metaNode.GetChild(embeddedResName))
            {
                embeddedRes->ImportMeta(*embeddedResMetaNode);
            }
        }
    }
}

void Resource::ExportMeta(MetaNode *metaNode) const
{
    Serializable::ExportMeta(metaNode);

    for(const auto &pair : m_nameToEmbeddedResource)
    {
        if(Resource *embeddedRes = pair.second)
        {
            const String &embeddedResName = pair.first;
            MetaNode embeddedResMeta = embeddedRes->GetMeta();
            embeddedResMeta.SetName(embeddedResName);
            metaNode->AddChild(embeddedResMeta, "Children");
        }
    }
}

void Resource::Import_(const Path &resourceFilepath)
{
    Import(resourceFilepath);
    Path importFilepath = MetaFilesManager::GetMetaFilepath(resourceFilepath);
    ImportMetaFromFile(importFilepath);  // Import Meta then

    EventEmitter<IEventsResource>::PropagateToListeners(
        &IEventsResource::OnImported, this);
}

void Resource::ClearEmbeddedResources()
{
    while(!m_embeddedResources.IsEmpty())
    {
        Resource *embeddedResource = m_embeddedResources.Back().Get();
        RemoveEmbeddedResource(embeddedResource);
    }

    m_nameToEmbeddedResource.Clear();
    m_embeddedResourceToName.Clear();
    m_GUIDToEmbeddedResource.Clear();
    m_nextNewEmbeddedResourceGUID = 1;
}

GUID::GUIDType Resource::GetNewEmbeddedResourceGUID() const
{
    auto nextNewGUID = m_nextNewEmbeddedResourceGUID;
    ++m_nextNewEmbeddedResourceGUID;
    return nextNewGUID;
}
