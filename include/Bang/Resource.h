#ifndef RESOURCE_H
#define RESOURCE_H

#include "Bang/Map.h"
#include "Bang/IGUIDable.h"
#include "Bang/EventEmitter.h"
#include "Bang/Serializable.h"
#include "Bang/EventListener.h"
#include "Bang/ResourceHandle.h"
#include "Bang/IEventsResource.h"

NAMESPACE_BANG_BEGIN

class Resource : public Serializable,
                 public EventEmitter<IEventsResource>
{
public:
    void RemoveEmbeddedResource(Resource *resource);
    void RemoveEmbeddedResource(const String &embeddedResourceName);
    void PropagateResourceChanged();

    Path GetResourceFilepath() const;
    Resource* GetParentResource() const;
    const Array<RH<Resource>>& GetEmbeddedResources() const;
    Resource* GetEmbeddedResource(const GUID &embeddedResGUID) const;
    Resource* GetEmbeddedResource(const String &embeddedResourceName) const;
    Resource* GetEmbeddedResource(GUID::GUIDType embeddedResGUID) const;
    String GetEmbeddedResourceName(GUID::GUIDType embeddedResGUID) const;

protected:
    Resource();
    virtual ~Resource();

    void ClearEmbeddedResources();

    // Serializable
    virtual void ImportMeta(const MetaNode &metaNode);
    virtual void ExportMeta(MetaNode *metaNode) const;

    // Resource
    virtual void Import(const Path &resourceFilepath) = 0;
    void Import_(const Path &resourceFilepath);

private:
    // Embedded resource related variables
    RH<Resource> p_parentResource;
    Array< RH<Resource> > m_embeddedResources;
    Map<String, Resource*> m_nameToEmbeddedResource;
    Map<Resource*, String> m_embeddedResourceToName;
    Map<GUID, Resource*> m_GUIDToEmbeddedResource;
    mutable GUID::GUIDType m_nextNewEmbeddedResourceGUID = 1;

    void AddEmbeddedResource(const String &embeddedResourceName,
                             Resource *embeddedResource);
    GUID::GUIDType GetNewEmbeddedResourceGUID() const;

    void SetParentResource(Resource *parentResource);

    friend class Resources;
};

#define RESOURCE(CLASSNAME) \
    SERIALIZABLE(CLASSNAME) \
    friend class Resources;

NAMESPACE_BANG_END

#endif // RESOURCE_H
