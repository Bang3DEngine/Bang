#ifndef IEVENTSFILETRACKER_H
#define IEVENTSFILETRACKER_H

#include "Bang/IEvents.h"

namespace Bang
{
class Path;

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
}

#endif  // IEVENTSFILETRACKER_H
