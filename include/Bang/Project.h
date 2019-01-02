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

    Path GetProjectDirectory() const;
    Path GetProjectAssetsFilepath() const;
    Path GetProjectFilepath() const;
    String GetProjectName() const;

    void SetProjectFilepath(const Path &projectFilepath);

    virtual bool OpenInitialScene() const;
    Path GetFirstFoundScenePath() const;

    // Serializable
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;

private:
    Path m_projectFilepath;
};
}

#endif  // PROJECT_H
