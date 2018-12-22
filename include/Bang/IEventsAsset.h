#ifndef IEVENTSASSET_H
#define IEVENTSASSET_H

#include "Bang/IEvents.h"

namespace Bang
{
class Asset;

class IEventsAsset
{
    IEVENTS(IEventsAsset);

public:
    virtual void OnAssetChanged(Asset *asset)
    {
        BANG_UNUSED(asset);
    }

    virtual void OnImported(Asset *asset)
    {
        BANG_UNUSED(asset);
    }
};
}

#endif  // IEVENTSASSET_H
