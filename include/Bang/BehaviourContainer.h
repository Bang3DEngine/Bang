#ifndef BEHAVIOURCONTAINER_H
#define BEHAVIOURCONTAINER_H

#include "Bang/BangDefines.h"
#include "Bang/Component.h"
#include "Bang/ComponentMacros.h"
#include "Bang/GUID.h"
#include "Bang/MetaNode.h"
#include "Bang/Path.h"
#include "Bang/String.h"

namespace Bang
{
class Behaviour;
class ICloneable;
class Library;

class BehaviourContainer : public Component
{
    COMPONENT(BehaviourContainer)

public:
    void SetSourceFilepath(const Path &sourceFilepath);
    void SetSourceFilepathGUID(const GUID &sourceFilepathGUID);
    Behaviour *CreateBehaviourInstance(Library *behavioursLibrary) const;

    void TryToSubstituteByBehaviourInstance();
    void SetInitializationModificationsMeta(const MetaNode &metaNode);
    void SubstituteByBehaviourInstance(Library *behavioursLibrary);
    void UpdateInformationFromHeaderIfNeeded();
    void ResetInitializationModificationsMetaNode();

    String GetBehaviourName() const;
    Path GetSourceFilepath() const;
    MetaNode *GetIntializationModificationMetaPtr();
    const GUID &GetSourceFilepathGUID() const;
    MetaNode GetInitializationMeta() const;
    const MetaNode &GetInitializationModificationsMeta() const;
    const ReflectStruct &GetBehaviourReflectStruct() const;

private:
    GUID m_sourceFilepathGUID;
    MetaNode m_initializationModificationsMeta;
    ReflectStruct m_behaviourReflectStruct;
    Time m_prevTimeHeaderChanged = Time::Zero();

    BehaviourContainer();
    virtual ~BehaviourContainer() override;

    // Component
    void OnPreStart() override;

    // ICloneable
    virtual void CloneInto(ICloneable *clone, bool cloneGUID) const override;

    // Serializable
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;
};
}

#endif  // BEHAVIOURCONTAINER_H
