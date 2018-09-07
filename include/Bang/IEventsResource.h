#ifndef IEVENTSRESOURCE_H
#define IEVENTSRESOURCE_H

#include "Bang/IEvents.h"

NAMESPACE_BANG_BEGIN

FORWARD class Resource;

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

NAMESPACE_BANG_END

#endif // IEVENTSRESOURCE_H
