#include "Bang/AnimatorStateMachine.h"

#include "Bang/MetaNode.h"
#include "Bang/MetaFilesManager.h"
#include "Bang/AnimatorStateMachineNode.h"
#include "Bang/AnimatorStateMachineConnection.h"

USING_NAMESPACE_BANG

AnimatorStateMachine::AnimatorStateMachine()
{
    AnimatorStateMachineNode *entryNode = CreateAndAddNode();
    entryNode->SetName("Entry");
}

AnimatorStateMachine::~AnimatorStateMachine()
{
    EventEmitter<IEventsDestroy>::PropagateToListeners(
                                        &IEventsDestroy::OnDestroyed, this);

    while (!m_nodes.IsEmpty())
    {
        delete m_nodes.Back();
        m_nodes.PopBack();
    }
}

AnimatorStateMachineNode *AnimatorStateMachine::CreateAndAddNode()
{
    AnimatorStateMachineNode *newSMNode = new AnimatorStateMachineNode(this);
    m_nodes.PushBack(newSMNode);

    EventEmitter<IEventsAnimatorStateMachine>::PropagateToListeners(
                &IEventsAnimatorStateMachine::OnNodeCreated,
                this,
                m_nodes.Size()-1,
                newSMNode);

    return newSMNode;
}

const AnimatorStateMachineNode *AnimatorStateMachine::GetNode(uint nodeIdx) const
{
    return const_cast<AnimatorStateMachine*>(this)->GetNode(nodeIdx);
}

AnimatorStateMachineNode *AnimatorStateMachine::GetNode(uint nodeIdx)
{
    if (nodeIdx < m_nodes.Size())
    {
        return m_nodes[nodeIdx];
    }
    return nullptr;
}

void AnimatorStateMachine::RemoveNode(AnimatorStateMachineNode *nodeToRemove)
{
    for (AnimatorStateMachineNode *node : m_nodes)
    {
        for (uint i = 0; i < node->GetConnections().Size();)
        {
            AnimatorStateMachineConnection *conn = node->GetConnection(i);
            if (conn->GetNodeTo()   == nodeToRemove ||
                conn->GetNodeFrom() == nodeToRemove)
            {
                node->RemoveConnection(conn);
            }
            else
            {
                ++i;
            }
        }
    }

    const uint idxToRemove = GetNodes().IndexOf(nodeToRemove);

    EventEmitter<IEventsAnimatorStateMachine>::PropagateToListeners(
                &IEventsAnimatorStateMachine::OnNodeRemoved,
                this, idxToRemove, nodeToRemove);
    m_nodes.Remove(nodeToRemove);
}

void AnimatorStateMachine::Clear()
{
    while (!m_nodes.IsEmpty())
    {
        RemoveNode( m_nodes.Back() );
    }
}

const Array<AnimatorStateMachineNode*>& AnimatorStateMachine::GetNodes() const
{
    return m_nodes;
}

void AnimatorStateMachine::Import(const Path &resourceFilepath)
{
    ImportMetaFromFile( MetaFilesManager::GetMetaFilepath(resourceFilepath) );
}

void AnimatorStateMachine::ImportMeta(const MetaNode &metaNode)
{
    Resource::ImportMeta(metaNode);

    if (metaNode.GetChildren().Size() >= 1)
    {
        Clear();

        // First just create the nodes (so that indices work nice)...
        for (uint i = 0; i < metaNode.GetChildren().Size(); ++i)
        {
            CreateAndAddNode();
        }

        // Now import nodes meta
        uint i = 0;
        for (const MetaNode &childMetaNode : metaNode.GetChildren())
        {
            AnimatorStateMachineNode *node = GetNode(i);
            node->ImportMeta(childMetaNode);
            ++i;
        }
    }
}

void AnimatorStateMachine::ExportMeta(MetaNode *metaNode) const
{
    Resource::ExportMeta(metaNode);

    for (const AnimatorStateMachineNode *smNode : GetNodes())
    {
        MetaNode smNodeMeta;
        smNode->ExportMeta(&smNodeMeta);
        metaNode->AddChild(smNodeMeta);
    }
}
