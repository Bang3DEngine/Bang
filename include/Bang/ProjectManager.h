#ifndef PROJECTMANAGER_H
#define PROJECTMANAGER_H

#include <vector>

#include "Bang/Array.tcc"
#include "Bang/Bang.h"
#include "Bang/BangDefines.h"
#include "Bang/EventEmitter.h"
#include "Bang/EventListener.h"
#include "Bang/IEventsProjectManager.h"
#include "Bang/String.h"

namespace Bang
{
class Path;
class Project;

class ProjectManager : public EventEmitter<IEventsProjectManager>
{
public:
    ProjectManager();
    virtual ~ProjectManager();

    virtual Project *OpenProject(const Path &projectFilepath);
    virtual bool CloseCurrentProject();

    Project *GetCurrentProject() const;
    static ProjectManager *GetInstance();

protected:
    void SetCurrentProject(Project *project);
    virtual Project *CreateNewProject() const;

private:
    Project *m_currentProject = nullptr;
};
}

#endif  // PROJECTMANAGER_H
