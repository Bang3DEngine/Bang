#include "Bang/Project.h"

#include <istream>

#include "Bang/Array.h"
#include "Bang/Array.tcc"
#include "Bang/Extensions.h"
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

bool Project::OpenInitialScene() const
{
    Path scenePath = GetInitialScenePath();
    SceneManager::LoadSceneInstantly(scenePath, false);
    return scenePath.IsFile();
}

Path Project::GetInitialScenePath() const
{
    Array<Path> sceneFilepaths = GetProjectAssetsFilepath().GetFiles(
        FindFlag::RECURSIVE, {Extensions::GetSceneExtension()});
    Paths::SortPathsByName(&sceneFilepaths);
    return !sceneFilepaths.IsEmpty() ? sceneFilepaths.Back() : Path::Empty();
}

void Project::ImportMeta(const MetaNode &metaNode)
{
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
    metaNode->Set("Physics_StepSleepTime",
                  Physics::GetInstance()->GetStepSleepTime().GetSeconds());
    metaNode->Set("Physics_MaxSubSteps",
                  Physics::GetInstance()->GetMaxSubSteps());
    metaNode->Set("Physics_Gravity", Physics::GetInstance()->GetGravity());
}
