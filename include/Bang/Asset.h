#ifndef ASSET_H
#define ASSET_H

#include "Bang/Path.h"
#include "Bang/Object.h"
#include "Bang/Resource.h"
#include "Bang/IToString.h"
#include "Bang/ResourceHandle.h"

NAMESPACE_BANG_BEGIN

#define ASSET(CLASSNAME) \
    RESOURCE(CLASSNAME) \
    friend class Asset;

class Asset : public Resource,
              public IToString,
              public EventEmitter<IEventsDestroy>
{
public:
    // IToString
    virtual String ToString() const override;

    // Serializable
    virtual void ImportXML(const XMLNode &xmlInfo) override;
    virtual void ExportXML(XMLNode *xmlInfo) const override;

protected:
    Asset() = default;
    virtual ~Asset() = default;
};

NAMESPACE_BANG_END

#endif // ASSET_H
