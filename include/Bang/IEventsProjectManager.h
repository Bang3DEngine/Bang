#ifndef IEVENTSPROJECTMANAGER_H
#define IEVENTSPROJECTMANAGER_H

#include "Bang/IEvents.h"

namespace Bang
{
class Project;
class IEventsProjectManager
{
    IEVENTS_NS(IEventsProjectManager);

public:
    virtual void OnProjectOpen(const Project *project)
    {
        BANG_UNUSED(project);
    }

    virtual void OnProjectClosed(const Project *project)
    {
        BANG_UNUSED(project);
    }
};
}

#endif  // IEVENTSPROJECTMANAGER_H
