#ifndef IEVENTSRESOURCE_H
#define IEVENTSRESOURCE_H

#include "Bang/IEvents.h"

namespace Bang
{
class Resource;

class IEventsResource
{
    IEVENTS(IEventsResource);

public:
    virtual void OnResourceChanged(Resource *res)
    {
        BANG_UNUSED(res);
    }

    virtual void OnImported(Resource *res)
    {
        BANG_UNUSED(res);
    }
};
}

#endif  // IEVENTSRESOURCE_H
