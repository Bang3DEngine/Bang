#include "Bang/Project.h"

#include <istream>

#include "Bang/Array.h"
#include "Bang/Array.tcc"
#include "Bang/Extensions.h"
#include "Bang/MetaFilesManager.h"
#include "Bang/MetaNode.h"
#include "Bang/MetaNode.tcc"
#include "Bang/Paths.h"
#include "Bang/Physics.h"
#include "Bang/SceneManager.h"
#include "Bang/Time.h"

using namespace Bang;

Project::Project()
{
}

Project::~Project()
{
}

void Project::Init()
{
    Array<Path> sceneFilepaths = GetProjectAssetsFilepath().GetFiles(
        FindFlag::RECURSIVE, {Extensions::GetSceneExtension()});
    Paths::SortPathsByName(&sceneFilepaths);

    if (sceneFilepaths.Size() >= 1)
    {
        SetInitialScenePath(sceneFilepaths.Front());
    }
}

Path Project::GetProjectDirectory() const
{
    return GetProjectFilepath().GetDirectory();
}

Path Project::GetProjectAssetsFilepath() const
{
    return Path(GetProjectDirectory().Append("Assets"));
}

Path Project::GetProjectFilepath() const
{
    return m_projectFilepath;
}

String Project::GetProjectName() const
{
    return GetProjectFilepath().GetName();
}

void Project::SetProjectFilepath(const Path &projectFilepath)
{
    m_projectFilepath = projectFilepath;
}

void Project::SetInitialSceneGUID(const GUID &initialSceneGUID)
{
    m_initialScenePathGUID = initialSceneGUID;
}

void Project::SetInitialScenePath(const Path &initialScenePath)
{
    SetInitialSceneGUID(MetaFilesManager::GetGUID(initialScenePath));
}

bool Project::OpenInitialScene() const
{
    Path scenePath = GetInitialScenePath();
    SceneManager::LoadSceneInstantly(scenePath, false);
    return scenePath.IsFile();
}

void Project::ExportToProjectFile()
{
    ExportMetaToFile(GetProjectFilepath());
}

Path Project::GetInitialScenePath() const
{
    return MetaFilesManager::GetFilepath(GetInitialScenePathGUID());
}

GUID Project::GetInitialScenePathGUID() const
{
    return m_initialScenePathGUID;
}

void Project::ImportMeta(const MetaNode &metaNode)
{
    if (metaNode.Contains("InitialSceneGUID"))
    {
        GUID initialSceneGUID = metaNode.Get<GUID>("InitialSceneGUID");
        SetInitialSceneGUID(initialSceneGUID);
    }

    if (metaNode.Contains("Physics_StepSleepTime"))
    {
        Physics::GetInstance()->SetStepSleepTime(
            Time::Seconds(metaNode.Get<float>("Physics_StepSleepTime")));
    }

    if (metaNode.Contains("Physics_MaxSubSteps"))
    {
        Physics::GetInstance()->SetMaxSubSteps(
            metaNode.Get<int>("Physics_MaxSubSteps"));
    }

    if (metaNode.Contains("Physics_Gravity"))
    {
        Physics::GetInstance()->SetGravity(
            metaNode.Get<Vector3>("Physics_Gravity"));
    }
}

void Project::ExportMeta(MetaNode *metaNode) const
{
    metaNode->SetName("Project");

    metaNode->Set("InitialSceneGUID", GetInitialScenePathGUID());

    metaNode->Set("Physics_StepSleepTime",
                  Physics::GetInstance()->GetStepSleepTime().GetSeconds());
    metaNode->Set("Physics_MaxSubSteps",
                  Physics::GetInstance()->GetMaxSubSteps());
    metaNode->Set("Physics_Gravity", Physics::GetInstance()->GetGravity());
}
