#ifndef BEHAVIOURCONTAINER_H
#define BEHAVIOURCONTAINER_H

#include "Bang/Path.h"
#include "Bang/MetaNode.h"
#include "Bang/Component.h"

FORWARD NAMESPACE_BANG_BEGIN
FORWARD NAMESPACE_BANG_END

NAMESPACE_BANG_BEGIN

FORWARD class Library;
FORWARD class MetaNode;
FORWARD class Behaviour;

class BehaviourContainer : public Component
{
    COMPONENT_WITH_FAST_DYNAMIC_CAST(BehaviourContainer)

public:
    void SetSourceFilepath(const Path &sourceFilepath);
    void SetSourceFilepathGUID(const GUID &sourceFilepathGUID);
    Behaviour* CreateBehaviourInstance(Library *behavioursLibrary) const;

    String GetBehaviourName() const;
    Path GetSourceFilepath() const;
    const GUID& GetSourceFilepathGUID() const;
    const MetaNode& GetInitializationMeta() const;

    void TryToSubstituteByBehaviourInstance();
    void SetInitializationMeta(const MetaNode &metaNode);
    void SubstituteByBehaviourInstance(Library *behavioursLibrary);

private:
    GUID m_sourceFilepathGUID;
    MetaNode m_initializationMeta;
    Behaviour *p_behaviour = nullptr;

    BehaviourContainer();
    virtual ~BehaviourContainer();

    // Component
    void OnPreStart() override;

    // ICloneable
    virtual void CloneInto(ICloneable *clone) const override;

    // Serializable
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;

    friend class ComponentFactory;
};

NAMESPACE_BANG_END

#endif // BEHAVIOURCONTAINER_H

