#ifndef ASSET_H
#define ASSET_H

#include <vector>

#include "Bang/Array.tcc"
#include "Bang/BangDefines.h"
#include "Bang/EventEmitter.h"
#include "Bang/EventEmitter.tcc"
#include "Bang/EventListener.tcc"
#include "Bang/IEventsDestroy.h"
#include "Bang/IToString.h"
#include "Bang/Resource.h"
#include "Bang/String.h"

NAMESPACE_BANG_BEGIN

FORWARD class IEventsDestroy;
FORWARD class MetaNode;

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
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;

protected:
    Asset() = default;
    virtual ~Asset() = default;
};

NAMESPACE_BANG_END

#endif // ASSET_H
