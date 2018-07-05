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

GUID::GUIDType Resource::GetNextEmbeddedFileGUID() const
{
    return 0;
}

Resource *Resource::GetEmbeddedResource(const GUID &embeddedFileGUID) const
{
    return GetEmbeddedResource(embeddedFileGUID.GetEmbeddedFileGUID());
}

Resource *Resource::GetEmbeddedResource(const String &embeddedResourceName) const
{
    (void) embeddedResourceName;
    return nullptr;
}

Resource* Resource::GetEmbeddedResource(GUID::GUIDType) const
{
    return nullptr;
}

String Resource::GetEmbeddedFileResourceName(GUID::GUIDType) const
{
    return "EmbeddedResource";
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
