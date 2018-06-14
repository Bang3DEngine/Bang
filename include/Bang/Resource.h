#ifndef RESOURCE_H
#define RESOURCE_H

#include "Bang/IGUIDable.h"
#include "Bang/EventEmitter.h"
#include "Bang/Serializable.h"
#include "Bang/EventListener.h"
#include "Bang/IEventsResource.h"

NAMESPACE_BANG_BEGIN

class Resource : public Serializable,
                 public EventEmitter<IEventsResource>
{
public:
    Path GetResourceFilepath() const;

    void PropagateResourceChanged();

    Resource* GetEmbeddedResource(const GUID &embeddedFileGUID) const;
    Resource* GetEmbeddedResource(const String &embeddedResourceName) const;
    virtual Resource* GetEmbeddedResource(GUID::GUIDType embeddedFileGUID) const;
    virtual String GetEmbeddedFileResourceName(GUID::GUIDType embeddedFileGUID) const;
    virtual GUID GetEmbeddedResourceGUID(const String &embeddedResourceName) const;

protected:
    Resource();
    virtual ~Resource();

    virtual GUID::GUIDType GetNextEmbeddedFileGUID() const;

    virtual void ImportXML(const XMLNode &xmlInfo);
    virtual void ExportXML(XMLNode *xmlInfo) const;

    void _Import(const Path &resourceFilepath);
    virtual void Import(const Path &resourceFilepath) = 0;

    friend class Resources;
};

#define RESOURCE(CLASSNAME) \
    SERIALIZABLE(CLASSNAME) \
    friend class Resources;

NAMESPACE_BANG_END

#endif // RESOURCE_H
