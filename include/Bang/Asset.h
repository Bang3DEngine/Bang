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

namespace Bang
{
class IEventsDestroy;
class MetaNode;

#define ASSET_ABSTRACT(CLASSNAME) \
    RESOURCE(CLASSNAME)           \
    friend class Asset;

#define ASSET(CLASSNAME)      \
    ASSET_ABSTRACT(CLASSNAME) \
    ICLONEABLE(CLASSNAME)

class Asset : public Resource,
              public IToString,
              public EventEmitterIEventsDestroyWithCheck
{
public:
    // IToString
    virtual String ToString() const override;

    // Serializable
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;

protected:
    Asset() = default;
    virtual ~Asset() override = default;
};
}

#endif  // ASSET_H
