#include "Bang/BehaviourContainer.h"

#include "Bang/MetaNode.h"
#include "Bang/Behaviour.h"
#include "Bang/Application.h"
#include "Bang/BehaviourManager.h"

USING_NAMESPACE_BANG

BehaviourContainer::BehaviourContainer()
{
}

BehaviourContainer::~BehaviourContainer()
{
}

void BehaviourContainer::OnPreStart()
{
    Component::OnPreStart();
    TryToSubstituteByBehaviourInstance();
}

void BehaviourContainer::OnGameObjectChanged(GameObject *previousGameObject,
                                             GameObject *newGameObject)
{
    Component::OnGameObjectChanged(previousGameObject, newGameObject);
    TryToSubstituteByBehaviourInstance();
}

void BehaviourContainer::SetSourceFilepath(const Path &sourceFilepath)
{
    if (sourceFilepath != GetSourceFilepath())
    {
        m_sourceFilepath = sourceFilepath;
    }
}

Behaviour *BehaviourContainer::CreateBehaviourInstance(Library *behavioursLibrary) const
{
    return BehaviourManager::CreateBehaviourInstance(GetBehaviourName(),
                                                     behavioursLibrary);
}

String BehaviourContainer::GetBehaviourName() const
{
    return GetSourceFilepath().GetName();
}

const Path &BehaviourContainer::GetSourceFilepath() const
{
    return m_sourceFilepath;
}

void BehaviourContainer::TryToSubstituteByBehaviourInstance()
{
    BehaviourManager *behaviourManager = BehaviourManager::GetActive();
    if ( behaviourManager &&
         behaviourManager->IsInstanceCreationAllowed() &&
        !IsWaitingToBeDestroyed() &&
        !GetBehaviourName().IsEmpty()
       )
    {
        Library *behLib = behaviourManager->GetBehavioursLibrary();
        SubstituteByBehaviourInstance(behLib);
    }
}

void BehaviourContainer::SubstituteByBehaviourInstance(Library *behavioursLibrary)
{
    if (Behaviour *behaviour = CreateBehaviourInstance(behavioursLibrary))
    {
        if (GetGameObject())
        {
            GetGameObject()->AddComponent(behaviour);
            Component::Destroy(this);
        }
    }
}

void BehaviourContainer::CloneInto(ICloneable *clone) const
{
    Component::CloneInto(clone);
    BehaviourContainer *bc = Cast<BehaviourContainer*>(clone);
    bc->SetSourceFilepath( GetSourceFilepath() );
}

void BehaviourContainer::ImportMeta(const MetaNode &metaNode)
{
    Component::ImportMeta(metaNode);

    if (metaNode.Contains("SourceFilepath"))
    {
        SetSourceFilepath( metaNode.Get<Path>("SourceFilepath") );
    }

    TryToSubstituteByBehaviourInstance();
}

void BehaviourContainer::ExportMeta(MetaNode *metaNode) const
{
    Component::ExportMeta(metaNode);

    metaNode->Set("SourceFilepath", GetSourceFilepath());
}
