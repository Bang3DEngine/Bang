#ifndef PROJECT_H
#define PROJECT_H

#include "Bang/BangDefines.h"
#include "Bang/GUID.h"
#include "Bang/MetaNode.h"
#include "Bang/Path.h"
#include "Bang/Serializable.h"
#include "Bang/String.h"

using namespace Bang;
namespace Bang
{
class Project : public Serializable
{
    SERIALIZABLE(Project)

public:
    Project();
    virtual ~Project() override;

    virtual void Init();

    virtual bool OpenInitialScene() const;
    void ExportToProjectFile();

    void SetProjectFilepath(const Path &projectFilepath);
    void SetInitialSceneGUID(const GUID &initialSceneGUID);
    void SetInitialScenePath(const Path &initialScenePath);

    Path GetProjectDirectory() const;
    Path GetProjectAssetsFilepath() const;
    Path GetProjectFilepath() const;
    String GetProjectName() const;
    Path GetInitialScenePath() const;
    GUID GetInitialScenePathGUID() const;

    // Serializable
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;

private:
    Path m_projectFilepath = Path::Empty();
    GUID m_initialScenePathGUID = GUID::Empty();
};
}

#endif  // PROJECT_H
