#include "Bang/BehaviourContainer.h"

#include "Bang/MetaNode.h"
#include "Bang/Behaviour.h"
#include "Bang/Application.h"
#include "Bang/BehaviourManager.h"

USING_NAMESPACE_BANG

BehaviourContainer::BehaviourContainer()
{
    CONSTRUCT_CLASS_ID(BehaviourContainer)
}

BehaviourContainer::~BehaviourContainer()
{
}

void BehaviourContainer::OnPreStart()
{
    Component::OnPreStart();
    TryToSubstituteByBehaviourInstance();
}

void BehaviourContainer::SetSourceFilepath(const Path &sourceFilepath)
{
    SetSourceFilepathGUID( MetaFilesManager::GetGUID(sourceFilepath) );
}

void BehaviourContainer::SetSourceFilepathGUID(const GUID &sourceFilepathGUID)
{
    if (sourceFilepathGUID != GetSourceFilepathGUID())
    {
        m_sourceFilepathGUID = sourceFilepathGUID;
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

Path BehaviourContainer::GetSourceFilepath() const
{
    return MetaFilesManager::GetFilepath( GetSourceFilepathGUID() );
}

const GUID &BehaviourContainer::GetSourceFilepathGUID() const
{
    return m_sourceFilepathGUID;
}

const MetaNode &BehaviourContainer::GetInitializationMeta() const
{
    return m_initializationMeta;
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

void BehaviourContainer::SetInitializationMeta(const MetaNode &metaNode)
{
    m_initializationMeta = metaNode;
}

void BehaviourContainer::SubstituteByBehaviourInstance(Library *behavioursLibrary)
{
    if (Behaviour *behaviour = CreateBehaviourInstance(behavioursLibrary))
    {
        behaviour->ImportMeta( GetInitializationMeta() );
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
    bc->SetSourceFilepathGUID( GetSourceFilepathGUID() );
    bc->SetInitializationMeta( GetInitializationMeta() );
}

void BehaviourContainer::ImportMeta(const MetaNode &metaNode)
{
    Component::ImportMeta(metaNode);

    if (metaNode.Contains("SourceFilepathGUID"))
    {
        SetSourceFilepathGUID( metaNode.Get<GUID>("SourceFilepathGUID") );
    }

    if (metaNode.Contains("InitializationMeta"))
    {
        String metaStr = metaNode.Get<String>("InitializationMeta");
        MetaNode initializationMeta;
        initializationMeta.Import(metaStr);
        SetInitializationMeta(initializationMeta);
    }
}

void BehaviourContainer::ExportMeta(MetaNode *metaNode) const
{
    Component::ExportMeta(metaNode);

    metaNode->Set("SourceFilepathGUID", GetSourceFilepathGUID());
    metaNode->Set("InitializationMeta", GetInitializationMeta().ToString());
}
