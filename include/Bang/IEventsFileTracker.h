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
        (void) addedPath;
    }

    virtual void OnPathModified(const Path &modifiedPath)
    {
        (void) modifiedPath;
    }

    virtual void OnPathRemoved(const Path &removedPath)
    {
        (void) removedPath;
    }
};

NAMESPACE_BANG_END

#endif // IEVENTSFILETRACKER_H
