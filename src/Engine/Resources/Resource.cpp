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

GUID Resource::GetEmbeddedResourceGUID(const String &embeddedResourceName) const
{
    (void) embeddedResourceName;
    return GUID::Empty();
}

void Resource::ImportXML(const XMLNode &xmlInfo)
{
    Serializable::ImportXML(xmlInfo);
}

void Resource::ExportXML(XMLNode *xmlInfo) const
{
    Serializable::ExportXML(xmlInfo);
}

void Resource::_Import(const Path &resourceFilepath)
{
    Path importFilepath = ImportFilesManager::GetImportFilepath(resourceFilepath);
    Import(resourceFilepath);          // Import from filepath
    ImportXMLFromFile(importFilepath); // Import XML then

    EventEmitter<IEventsResource>::PropagateToListeners(
                &IEventsResource::OnImported, this);
}
