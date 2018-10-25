#include "Bang/AnimatorStateMachineLayer.h"

#include <sys/types.h>

#include "Bang/AnimatorStateMachine.h"
#include "Bang/AnimatorStateMachineNode.h"
#include "Bang/AnimatorStateMachineTransition.h"
#include "Bang/AnimatorStateMachineTransitionCondition.h"
#include "Bang/AnimatorStateMachineVariable.h"
#include "Bang/EventEmitter.tcc"
#include "Bang/IEventsAnimatorStateMachineLayer.h"
#include "Bang/IEventsDestroy.h"
#include "Bang/MetaFilesManager.h"
#include "Bang/MetaNode.h"
#include "Bang/MetaNode.tcc"
#include "Bang/Path.h"

using namespace Bang;

AnimatorStateMachineLayer::AnimatorStateMachineLayer()
{
    AnimatorStateMachineNode *entryNode = CreateAndAddNode();
    entryNode->SetName("Entry");
}

AnimatorStateMachineLayer::~AnimatorStateMachineLayer()
{
    EventEmitter<IEventsDestroy>::PropagateToListeners(
        &IEventsDestroy::OnDestroyed, this);
    Clear();
}

AnimatorStateMachineNode *AnimatorStateMachineLayer::CreateAndAddNode()
{
    AnimatorStateMachineNode *newSMNode = new AnimatorStateMachineNode();
    newSMNode->SetLayer(this);
    m_nodes.PushBack(newSMNode);

    if (!GetEntryNode())
    {
        SetEntryNode(newSMNode);
    }

    EventEmitter<IEventsAnimatorStateMachineLayer>::PropagateToListeners(
        &IEventsAnimatorStateMachineLayer::OnNodeCreated,
        m_nodes.Size() - 1,
        newSMNode);

    return newSMNode;
}

const AnimatorStateMachineNode *AnimatorStateMachineLayer::GetNode(
    uint nodeIdx) const
{
    return const_cast<AnimatorStateMachineLayer *>(this)->GetNode(nodeIdx);
}

AnimatorStateMachineNode *AnimatorStateMachineLayer::GetNode(uint nodeIdx)
{
    if (nodeIdx < m_nodes.Size())
    {
        return m_nodes[nodeIdx];
    }
    return nullptr;
}

void AnimatorStateMachineLayer::RemoveNode(
    AnimatorStateMachineNode *nodeToRemove)
{
    for (AnimatorStateMachineNode *node : m_nodes)
    {
        for (uint i = 0; i < node->GetTransitions().Size();)
        {
            AnimatorStateMachineTransition *conn = node->GetTransition(i);
            if (conn->GetNodeTo() == nodeToRemove ||
                conn->GetNodeFrom() == nodeToRemove)
            {
                node->RemoveTransition(conn);
            }
            else
            {
                ++i;
            }
        }
    }

    const uint idxToRemove = GetNodes().IndexOf(nodeToRemove);

    EventEmitter<IEventsAnimatorStateMachineLayer>::PropagateToListeners(
        &IEventsAnimatorStateMachineLayer::OnNodeRemoved,
        idxToRemove,
        nodeToRemove);

    m_nodes.Remove(nodeToRemove);
    delete nodeToRemove;

    if (idxToRemove == GetEntryNodeIdx())
    {
        SetEntryNodeIdx(0);
    }
}

void AnimatorStateMachineLayer::SetEntryNode(
    AnimatorStateMachineNode *entryNode)
{
    SetEntryNodeIdx(GetNodes().IndexOf(entryNode));
}

void AnimatorStateMachineLayer::SetEntryNodeIdx(uint entryNodeIdx)
{
    if (entryNodeIdx < GetNodes().Size())
    {
        m_entryNodeIdx = entryNodeIdx;
    }
    else
    {
        m_entryNodeIdx = -1u;
    }
}

void AnimatorStateMachineLayer::SetLayerName(const String &layerName)
{
    m_layerName = layerName;
}

void AnimatorStateMachineLayer::SetBoneName(const String &boneName)
{
    m_boneName = boneName;
}

void AnimatorStateMachineLayer::SetStateMachine(
    AnimatorStateMachine *stateMachine)
{
    p_stateMachine.Set(stateMachine);
}

AnimatorStateMachineNode *AnimatorStateMachineLayer::GetEntryNode() const
{
    if (GetEntryNodeIdx() < GetNodes().Size())
    {
        return GetNodes()[GetEntryNodeIdx()];
    }
    return nullptr;
}

AnimatorStateMachineNode *AnimatorStateMachineLayer::GetEntryNodeOrFirstFound()
    const
{
    if (AnimatorStateMachineNode *entryNode = GetEntryNode())
    {
        return entryNode;
    }
    return ((m_nodes.Size() >= 1) ? m_nodes.Front() : nullptr);
}

uint AnimatorStateMachineLayer::GetEntryNodeIdx() const
{
    return m_entryNodeIdx;
}

void AnimatorStateMachineLayer::Clear()
{
    while (!m_nodes.IsEmpty())
    {
        RemoveNode(m_nodes.Back());
    }
}

const String &AnimatorStateMachineLayer::GetLayerName() const
{
    return m_layerName;
}

const String &AnimatorStateMachineLayer::GetBoneName() const
{
    return m_boneName;
}

AnimatorStateMachine *AnimatorStateMachineLayer::GetStateMachine() const
{
    return p_stateMachine.Get();
}

const Array<AnimatorStateMachineNode *> &AnimatorStateMachineLayer::GetNodes()
    const
{
    return m_nodes;
}

void AnimatorStateMachineLayer::ImportMeta(const MetaNode &metaNode)
{
    Serializable::ImportMeta(metaNode);

    if (metaNode.Contains("LayerName"))
    {
        SetLayerName(metaNode.Get<String>("LayerName"));
    }

    if (metaNode.Contains("BoneName"))
    {
        SetBoneName(metaNode.Get<String>("BoneName"));
    }

    if (metaNode.Contains("EntryNodeIdx"))
    {
        SetEntryNodeIdx(metaNode.Get<uint>("EntryNodeIdx"));
    }

    if (metaNode.GetChildren("Nodes").Size() >= 1)
    {
        Clear();

        // First just create the nodes (so that indices work nice)...
        {
            const auto &childrenMetaNodes = metaNode.GetChildren("Nodes");
            for (uint i = 0; i < childrenMetaNodes.Size(); ++i)
            {
                CreateAndAddNode();
            }

            // Now import nodes meta
            uint i = 0;
            for (const MetaNode &childMetaNode : childrenMetaNodes)
            {
                AnimatorStateMachineNode *node = GetNode(i);
                node->ImportMeta(childMetaNode);
                ++i;
            }
        }
    }
}

void AnimatorStateMachineLayer::ExportMeta(MetaNode *metaNode) const
{
    Serializable::ExportMeta(metaNode);

    metaNode->Set("LayerName", GetLayerName());
    metaNode->Set("BoneName", GetBoneName());
    metaNode->Set("EntryNodeIdx", GetEntryNodeIdx());

    for (const AnimatorStateMachineNode *smNode : GetNodes())
    {
        MetaNode smNodeMeta = smNode->GetMeta();
        metaNode->AddChild(smNodeMeta, "Nodes");
    }
}
