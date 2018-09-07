#ifndef IEVENTSFILETRACKER_H
#define IEVENTSFILETRACKER_H

#include "Bang/IEvents.h"

NAMESPACE_BANG_BEGIN

FORWARD class Path;

class IEventsFileTracker
{
    IEVENTS(IEventsFileTracker);

public:
    virtual void OnPathAdded(const Path &addedPath)
    {
        BANG_UNUSED(addedPath);
    }

    virtual void OnPathModified(const Path &modifiedPath)
    {
        BANG_UNUSED(modifiedPath);
    }

    virtual void OnPathRemoved(const Path &removedPath)
    {
        BANG_UNUSED(removedPath);
    }
};

NAMESPACE_BANG_END

#endif // IEVENTSFILETRACKER_H
