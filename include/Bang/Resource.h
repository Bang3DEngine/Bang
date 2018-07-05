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
    template<class T>
    void AddEmbeddedResource(const String &embeddedResourceName,
                             Resource *embeddedResource);
    void RemoveEmbeddedResource(Resource *resource);
    void RemoveEmbeddedResource(const String &embeddedResourceName);
    void SetParentResource(Resource *parentResource);
    void PropagateResourceChanged();

    Path GetResourceFilepath() const;
    Resource* GetParentResource() const;
    Resource* GetEmbeddedResource(const GUID &embeddedResGUID) const;
    Resource* GetEmbeddedResource(const String &embeddedResourceName) const;
    Resource* GetEmbeddedResource(GUID::GUIDType embeddedResGUID) const;
    String GetEmbeddedResourceName(GUID::GUIDType embeddedResGUID) const;

protected:
    Resource();
    virtual ~Resource();

    virtual void ImportXML(const XMLNode &xmlInfo);
    virtual void ExportXML(XMLNode *xmlInfo) const;

    void Import_(const Path &resourceFilepath);
    virtual void Import(const Path &resourceFilepath) = 0;


private:
    Map<String, RH<Resource>> m_nameToEmbeddedResource;
    Map<Resource*, String> m_embeddedResourceToName;
    Map<GUID, Resource*> m_GUIDToEmbeddedResource;
    RH<Resource> p_parentResource;

    friend class Resources;
};

template<class T>
void Resource::AddEmbeddedResource(const String &embeddedResourceName,
                                   Resource *embeddedResource)
{
    m_nameToEmbeddedResource.Add(embeddedResourceName,
                                 RH<Resource>(embeddedResource));
    m_embeddedResourceToName.Add(embeddedResource, embeddedResourceName);
    m_GUIDToEmbeddedResource.Add(embeddedResource->GetGUID(),
                                                 embeddedResource);

    RH<Resource> prevParentResource( embeddedResource->GetParentResource() );
    embeddedResource->SetParentResource(embeddedResource);

    if (prevParentResource)
    {
        prevParentResource.Get()->RemoveEmbeddedResource(embeddedResource);
    }
}

#define RESOURCE(CLASSNAME) \
    SERIALIZABLE(CLASSNAME) \
    friend class Resources;

NAMESPACE_BANG_END

#endif // RESOURCE_H
