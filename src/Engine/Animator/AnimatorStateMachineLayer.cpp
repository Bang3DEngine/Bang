#include "Bang/AnimatorStateMachineLayer.h"

#include <sys/types.h>

#include "Bang/Animator.h"
#include "Bang/AnimatorLayerMask.h"
#include "Bang/AnimatorStateMachine.h"
#include "Bang/AnimatorStateMachineBlendTreeNode.h"
#include "Bang/AnimatorStateMachineNode.h"
#include "Bang/AnimatorStateMachineTransition.h"
#include "Bang/AnimatorStateMachineTransitionCondition.h"
#include "Bang/AnimatorStateMachineVariable.h"
#include "Bang/Assets.h"
#include "Bang/EventEmitter.tcc"
#include "Bang/GameObject.h"
#include "Bang/IEventsAnimatorStateMachineLayer.h"
#include "Bang/IEventsDestroy.h"
#include "Bang/MetaFilesManager.h"
#include "Bang/MetaNode.h"
#include "Bang/MetaNode.tcc"
#include "Bang/Path.h"

using namespace Bang;

AnimatorStateMachineLayer::AnimatorStateMachineLayer()
{
    AnimatorStateMachineNode *entryNode = new AnimatorStateMachineNode();
    AddNode(entryNode);
    entryNode->SetName("Entry");
}

AnimatorStateMachineLayer::~AnimatorStateMachineLayer()
{
    EventEmitter<IEventsDestroy>::PropagateToListeners(
        &IEventsDestroy::OnDestroyed, this);
    Clear();
}

void AnimatorStateMachineLayer::AddNode(AnimatorStateMachineNode *newSMNode)
{
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
        m_entryNodeIdx = SCAST<uint>(-1);
    }
}

void AnimatorStateMachineLayer::SetLayerName(const String &layerName)
{
    m_layerName = layerName;
}

void AnimatorStateMachineLayer::SetLayerMask(AnimatorLayerMask *layerMask)
{
    if (layerMask != GetLayerMask())
    {
        m_layerMask.Set(layerMask);
    }
}

void AnimatorStateMachineLayer::SetEnabled(bool enabled)
{
    m_enabled = enabled;
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
    SetLayerMask(nullptr);
}

bool AnimatorStateMachineLayer::GetEnabled() const
{
    return m_enabled;
}

const String &AnimatorStateMachineLayer::GetLayerName() const
{
    return m_layerName;
}

AnimatorLayerMask *AnimatorStateMachineLayer::GetLayerMask() const
{
    return m_layerMask.Get();
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

    Clear();

    if (metaNode.Contains("LayerName"))
    {
        SetLayerName(metaNode.Get<String>("LayerName"));
    }

    if (metaNode.Contains("LayerMask"))
    {
        AH<AnimatorLayerMask> layerMask =
            Assets::Load<AnimatorLayerMask>(metaNode.Get<GUID>("LayerMask"));
        SetLayerMask(layerMask.Get());
    }

    if (metaNode.Contains("Enabled"))
    {
        SetEnabled(metaNode.Get<bool>("Enabled"));
    }

    {
        const auto &childrenMetaNodes = metaNode.GetChildren("Nodes");

        // First just create the nodes (so that indices work nice)...
        for (uint i = 0; i < childrenMetaNodes.Size(); ++i)
        {
            const MetaNode &childMetaNode = childrenMetaNodes[i];
            bool isBlendTreeNode =
                (childMetaNode.GetName() ==
                 AnimatorStateMachineBlendTreeNode::GetClassNameStatic());
            AnimatorStateMachineNode *newNode =
                isBlendTreeNode ? new AnimatorStateMachineBlendTreeNode()
                                : new AnimatorStateMachineNode();
            AddNode(newNode);
        }

        // Now import nodes meta
        for (uint i = 0; i < childrenMetaNodes.Size(); ++i)
        {
            const MetaNode &childMetaNode = childrenMetaNodes[i];
            AnimatorStateMachineNode *node = GetNode(i);
            node->ImportMeta(childMetaNode);
        }
    }

    if (metaNode.Contains("EntryNodeIdx"))
    {
        SetEntryNodeIdx(metaNode.Get<uint>("EntryNodeIdx"));
    }
}

void AnimatorStateMachineLayer::ExportMeta(MetaNode *metaNode) const
{
    Serializable::ExportMeta(metaNode);

    metaNode->Set("LayerName", GetLayerName());
    metaNode->Set("EntryNodeIdx", GetEntryNodeIdx());
    metaNode->Set("Enabled", GetEnabled());
    metaNode->Set("LayerMask",
                  GetLayerMask() ? GetLayerMask()->GetGUID() : GUID::Empty());

    metaNode->CreateChildrenContainer("Nodes");
    for (const AnimatorStateMachineNode *smNode : GetNodes())
    {
        MetaNode smNodeMeta = smNode->GetMeta();
        metaNode->AddChild(smNodeMeta, "Nodes");
    }
}
