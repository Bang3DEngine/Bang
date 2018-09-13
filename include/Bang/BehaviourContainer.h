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
    COMPONENT(BehaviourContainer)

public:
    void SetSourceFilepath(const Path &sourceFilepath);
    Behaviour* CreateBehaviourInstance(Library *behavioursLibrary) const;

    String GetBehaviourName() const;
    const Path& GetSourceFilepath() const;
    const MetaNode& GetInitializationMeta() const;

    void TryToSubstituteByBehaviourInstance();
    void SetInitializationMeta(const MetaNode &metaNode);
    void SubstituteByBehaviourInstance(Library *behavioursLibrary);

private:
    Path m_sourceFilepath;
    MetaNode m_initializationMeta;
    Behaviour *p_behaviour = nullptr;

    BehaviourContainer();
    virtual ~BehaviourContainer();

    // Component
    void OnPreStart() override;
    void OnGameObjectChanged(GameObject *previousGameObject,
                             GameObject *newGameObject) override;

    // ICloneable
    virtual void CloneInto(ICloneable *clone) const override;

    // Serializable
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;

    friend class ComponentFactory;
};

NAMESPACE_BANG_END

#endif // BEHAVIOURCONTAINER_H

