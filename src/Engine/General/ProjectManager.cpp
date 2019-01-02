#include "Bang/ProjectManager.h"

#include <ostream>

#include "Bang/Application.h"
#include "Bang/Debug.h"
#include "Bang/Extensions.h"
#include "Bang/File.h"
#include "Bang/MetaFilesManager.h"
#include "Bang/Path.h"
#include "Bang/Paths.h"
#include "Bang/Project.h"
#include "Bang/StreamOperators.h"
#include "Bang/String.h"

using namespace Bang;

ProjectManager::ProjectManager()
{
}

ProjectManager::~ProjectManager()
{
    CloseCurrentProject();
}

Project *ProjectManager::OpenProject(const Path &projectFilepath)
{
    if (CloseCurrentProject())
    {
        m_currentProject = CreateNewProject();
        Project *currentProject = GetCurrentProject();
        currentProject->SetProjectFilepath(projectFilepath);

        Paths::SetProjectRoot(currentProject->GetProjectDirectory());

        Path assetsDir = currentProject->GetProjectAssetsFilepath();
        MetaFilesManager::CreateMissingMetaFiles(assetsDir);
        MetaFilesManager::LoadMetaFilepathGUIDs(assetsDir);

        currentProject->Init();
        currentProject->ImportMetaFromFile(projectFilepath);
        currentProject->OpenInitialScene();

        EventEmitter<IEventsProjectManager>::PropagateToListeners(
            &IEventsProjectManager::OnProjectOpen, GetCurrentProject());
    }
    return GetCurrentProject();
}

Project *ProjectManager::GetCurrentProject() const
{
    return m_currentProject;
}

bool ProjectManager::CloseCurrentProject()
{
    if (GetCurrentProject())
    {
        delete m_currentProject;
        m_currentProject = nullptr;
    }
    return true;
}

ProjectManager *ProjectManager::GetInstance()
{
    Application *app = Application::GetInstance();
    return app ? app->GetProjectManager() : nullptr;
}

void ProjectManager::SetCurrentProject(Project *project)
{
    m_currentProject = project;
}

Project *ProjectManager::CreateNewProject() const
{
    return new Project();
}
