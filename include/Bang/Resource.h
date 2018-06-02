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
    virtual GUID::GUIDType GetNextInsideFileGUID() const;

protected:
    Resource();
    virtual ~Resource();

    Resource* GetInsideFileResource(const GUID &insideFileGUID) const;
    virtual Resource* GetInsideFileResource(GUID::GUIDType insideFileGUID) const;
    virtual String GetInsideFileResourceName(GUID::GUIDType insideFileGUID) const;

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
