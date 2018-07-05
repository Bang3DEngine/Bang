#include "Bang/Resource.h"

#include "Bang/Resources.h"
#include "Bang/ImportFilesManager.h"

USING_NAMESPACE_BANG

// IResource
Resource::Resource()
{
}

Resource::~Resource()
{
}

void Resource::RemoveEmbeddedResource(Resource *embeddedResource)
{
     m_nameToEmbeddedResource.RemoveValues(RH<Resource>(embeddedResource));
     m_embeddedResourceToName.Remove(embeddedResource);
     m_GUIDToEmbeddedResource.RemoveValues(embeddedResource);
     embeddedResource->SetParentResource(nullptr);
}
void Resource::RemoveEmbeddedResource(const String &embeddedResourceName)
{
    if (m_nameToEmbeddedResource.ContainsKey(embeddedResourceName))
    {
        Resource *embeddedResource = m_nameToEmbeddedResource.Get(
                                        embeddedResourceName).Get();
        RemoveEmbeddedResource(embeddedResource);
    }
}

void Resource::SetParentResource(Resource *parentResource)
{
    p_parentResource.Set(parentResource);
}

Resource* Resource::GetParentResource() const
{
    return p_parentResource.Get();
}

void Resource::PropagateResourceChanged()
{
    EventEmitter<IEventsResource>::PropagateToListeners(
                                        &IEventsResource::OnResourceChanged,
                                        this);
}

// Resource
Path Resource::GetResourceFilepath() const
{
    return ImportFilesManager::GetFilepath( GetGUID() );
}

Resource *Resource::GetEmbeddedResource(const GUID &embeddedResGUID) const
{
    if (m_GUIDToEmbeddedResource.ContainsKey(embeddedResGUID))
    {
        Resource *embeddedResource = m_GUIDToEmbeddedResource.Get(embeddedResGUID);
        return embeddedResource;
    }
    return nullptr;
}

Resource *Resource::GetEmbeddedResource(const String &embeddedResourceName) const
{
    if (m_nameToEmbeddedResource.ContainsKey(embeddedResourceName))
    {
        Resource *embeddedResource =
                    m_nameToEmbeddedResource.Get(embeddedResourceName).Get();
        return embeddedResource;
    }
    return nullptr;
}

Resource* Resource::GetEmbeddedResource(GUID::GUIDType embeddedResGUID) const
{
    GUID embeddedResourceFullGUID = GetGUID().WithEmbeddedResourceGUID(embeddedResGUID);
    return GetEmbeddedResource(embeddedResourceFullGUID);
}

String Resource::GetEmbeddedResourceName(GUID::GUIDType embeddedResGUID) const
{
    Resource *embeddedResource = GetEmbeddedResource(embeddedResGUID);
    if (embeddedResource)
    {
        if (m_embeddedResourceToName.ContainsKey(embeddedResource))
        {
            return m_embeddedResourceToName.Get(embeddedResource);
        }
    }
    return "";
}

void Resource::ImportXML(const XMLNode &xmlInfo)
{
    Serializable::ImportXML(xmlInfo);
}

void Resource::ExportXML(XMLNode *xmlInfo) const
{
    Serializable::ExportXML(xmlInfo);
}

void Resource::Import_(const Path &resourceFilepath)
{
    Import(resourceFilepath);
    Path importFilepath = ImportFilesManager::GetImportFilepath(resourceFilepath);
    ImportXMLFromFile(importFilepath); // Import XML then

    EventEmitter<IEventsResource>::PropagateToListeners(
                &IEventsResource::OnImported, this);
}

GUID::GUIDType Resource::GetNewEmbeddedResourceGUID() const
{
    auto nextNewGUID = m_nextNewEmbeddedResourceGUID;
    ++m_nextNewEmbeddedResourceGUID;
    return nextNewGUID;
}
