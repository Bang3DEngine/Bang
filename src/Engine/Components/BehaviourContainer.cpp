#include "Bang/BehaviourContainer.h"

#include "Bang/BangPreprocessor.h"
#include "Bang/Behaviour.h"
#include "Bang/BehaviourManager.h"
#include "Bang/ClassDB.h"
#include "Bang/GameObject.h"
#include "Bang/ICloneable.h"
#include "Bang/MetaFilesManager.h"
#include "Bang/MetaNode.h"
#include "Bang/MetaNode.tcc"
#include "Bang/StreamOperators.h"

using namespace Bang;

BehaviourContainer::BehaviourContainer()
{
    SET_INSTANCE_CLASS_ID(BehaviourContainer)
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
    SetSourceFilepathGUID(MetaFilesManager::GetGUID(sourceFilepath));
}

void BehaviourContainer::SetSourceFilepathGUID(const GUID &sourceFilepathGUID)
{
    if (sourceFilepathGUID != GetSourceFilepathGUID())
    {
        m_sourceFilepathGUID = sourceFilepathGUID;
        m_prevTimeHeaderChanged = Time::Zero();
        m_behaviourReflectStruct = ReflectStruct();
        ResetInitializationModificationsMetaNode();
        UpdateInformationFromHeaderIfNeeded();
    }
}

Behaviour *BehaviourContainer::CreateBehaviourInstance(
    Library *behavioursLibrary) const
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
    return MetaFilesManager::GetFilepath(GetSourceFilepathGUID());
}

MetaNode *BehaviourContainer::GetIntializationModificationMetaPtr()
{
    return &m_initializationModificationsMeta;
}

const GUID &BehaviourContainer::GetSourceFilepathGUID() const
{
    return m_sourceFilepathGUID;
}

MetaNode BehaviourContainer::GetInitializationMeta() const
{
    MetaNode metaNode = GetBehaviourReflectStruct().GetMeta();
    metaNode.Import(GetInitializationModificationsMeta());
    return metaNode;
}

const MetaNode &BehaviourContainer::GetInitializationModificationsMeta() const
{
    return m_initializationModificationsMeta;
}

const ReflectStruct &BehaviourContainer::GetBehaviourReflectStruct() const
{
    return m_behaviourReflectStruct;
}

void BehaviourContainer::TryToSubstituteByBehaviourInstance()
{
    BehaviourManager *behaviourManager = BehaviourManager::GetActive();
    if (behaviourManager && behaviourManager->IsInstanceCreationAllowed() &&
        !IsWaitingToBeDestroyed() && !GetBehaviourName().IsEmpty())
    {
        Library *behLib = behaviourManager->GetBehavioursLibrary();
        SubstituteByBehaviourInstance(behLib);
    }
}

void BehaviourContainer::SetInitializationModificationsMeta(
    const MetaNode &metaNode)
{
    m_initializationModificationsMeta = metaNode;
}

void BehaviourContainer::SubstituteByBehaviourInstance(
    Library *behavioursLibrary)
{
    if (Behaviour *behaviour = CreateBehaviourInstance(behavioursLibrary))
    {
        UpdateInformationFromHeaderIfNeeded();
        behaviour->ImportMeta(GetInitializationMeta());
        behaviour->ImportMeta(GetInitializationModificationsMeta());
        if (GetGameObject())
        {
            GetGameObject()->AddComponent(behaviour);
            Component::Destroy(this);
        }
    }
}

void BehaviourContainer::UpdateInformationFromHeaderIfNeeded()
{
    if (GetSourceFilepath().IsFile())
    {
        Path headerPath = GetSourceFilepath().WithExtension("h");
        if (headerPath.IsFile())
        {
            Time timeHeaderChanged = headerPath.GetModificationTime();
            bool hasHeaderChanged =
                (timeHeaderChanged > m_prevTimeHeaderChanged);
            if (hasHeaderChanged)
            {
                Array<ReflectStruct> reflStructs =
                    BangPreprocessor::GetReflectStructs(headerPath);
                if (reflStructs.Size() >= 1)
                {
                    m_behaviourReflectStruct = reflStructs.Front();
                }
                else
                {
                    m_behaviourReflectStruct = ReflectStruct();
                }

                m_prevTimeHeaderChanged = timeHeaderChanged;
            }
        }
    }
}

void BehaviourContainer::ResetInitializationModificationsMetaNode()
{
    SetInitializationModificationsMeta(MetaNode());
}

void BehaviourContainer::CloneInto(ICloneable *clone, bool cloneGUID) const
{
    Component::CloneInto(clone, cloneGUID);
    BehaviourContainer *bc = SCAST<BehaviourContainer *>(clone);
    bc->SetSourceFilepathGUID(GetSourceFilepathGUID());
    bc->SetInitializationModificationsMeta(
        GetInitializationModificationsMeta());
}

void BehaviourContainer::ImportMeta(const MetaNode &metaNode)
{
    Component::ImportMeta(metaNode);

    if (metaNode.Contains("SourceFilepathGUID"))
    {
        SetSourceFilepathGUID(metaNode.Get<GUID>("SourceFilepathGUID"));
    }

    if (metaNode.Contains("InitializationModificationsMeta"))
    {
        String metaStr =
            metaNode.Get<String>("InitializationModificationsMeta");
        MetaNode initializationModificationsMeta;
        initializationModificationsMeta.Import(metaStr);
        SetInitializationModificationsMeta(initializationModificationsMeta);
    }
}

void BehaviourContainer::ExportMeta(MetaNode *metaNode) const
{
    Component::ExportMeta(metaNode);

    metaNode->Set("SourceFilepathGUID", GetSourceFilepathGUID());
    metaNode->Set("InitializationModificationsMeta",
                  GetInitializationModificationsMeta().ToString());
}
