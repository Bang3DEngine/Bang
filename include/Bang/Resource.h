#ifndef RESOURCE_H
#define RESOURCE_H

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
    void SetParentResource(Resource *parentResource);
    void PropagateResourceChanged();

    Path GetResourceFilepath() const;
    Resource* GetParentResource() const;
    Resource* GetEmbeddedResource(const GUID &embeddedFileGUID) const;
    virtual Resource* GetEmbeddedResource(const String &embeddedResourceName) const;
    virtual Resource* GetEmbeddedResource(GUID::GUIDType embeddedFileGUID) const;
    virtual String GetEmbeddedFileResourceName(GUID::GUIDType embeddedFileGUID) const;

protected:
    Resource();
    virtual ~Resource();

    virtual GUID::GUIDType GetNextEmbeddedFileGUID() const;

    virtual void ImportXML(const XMLNode &xmlInfo);
    virtual void ExportXML(XMLNode *xmlInfo) const;

    void Import_(const Path &resourceFilepath);
    virtual void Import(const Path &resourceFilepath) = 0;


private:
    RH<Resource> p_parentResource;

    friend class Resources;
};

#define RESOURCE(CLASSNAME) \
    SERIALIZABLE(CLASSNAME) \
    friend class Resources;

NAMESPACE_BANG_END

#endif // RESOURCE_H
