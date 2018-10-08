#include "Bang/AnimatorStateMachine.h"

#include "Bang/MetaNode.h"
#include "Bang/MetaFilesManager.h"
#include "Bang/AnimatorStateMachineNode.h"

USING_NAMESPACE_BANG

AnimatorStateMachine::AnimatorStateMachine()
{
    AnimatorStateMachineNode *entryNode = CreateAndAddNode();
    entryNode->SetName("Entry");
}

AnimatorStateMachine::~AnimatorStateMachine()
{
}

uint AnimatorStateMachine::GetCurrentNodeIndex() const
{
    return m_currentNodeIndex;
}

void AnimatorStateMachine::CreateNodeInto(AnimatorStateMachineNode *node)
{
    node->SetAnimatorStateMachine(this);
}

AnimatorStateMachineNode *AnimatorStateMachine::CreateAndAddNode()
{
    AnimatorStateMachineNode smNode;
    CreateNodeInto(&smNode);
    m_nodes.PushBack(smNode);

    AnimatorStateMachineNode *newNode = &m_nodes.Back();

    EventEmitter<IEventsAnimatorStateMachine>::PropagateToListeners(
                &IEventsAnimatorStateMachine::OnNodeCreated,
                this,
                m_nodes.Size()-1,
                newNode);

    return newNode;
}

AnimatorStateMachineNode *AnimatorStateMachine::GetCurrentNode()
{
    if (GetCurrentNodeIndex() < GetNodes().Size())
    {
        return GetNode( GetCurrentNodeIndex() );
    }
    return nullptr;
}

const AnimatorStateMachineNode *AnimatorStateMachine::GetCurrentNode() const
{
    return const_cast<AnimatorStateMachine*>(this)->GetCurrentNode();
}

const AnimatorStateMachineNode *AnimatorStateMachine::GetNode(uint nodeIdx) const
{
    return const_cast<AnimatorStateMachine*>(this)->GetNode(nodeIdx);
}

AnimatorStateMachineNode *AnimatorStateMachine::GetNode(uint nodeIdx)
{
    if (nodeIdx < m_nodes.Size())
    {
        return &(m_nodes[nodeIdx]);
    }
    return nullptr;
}

void AnimatorStateMachine::RemoveNode(uint idxToRemove)
{
    ASSERT(idxToRemove < m_nodes.Size());

    EventEmitter<IEventsAnimatorStateMachine>::PropagateToListeners(
                &IEventsAnimatorStateMachine::OnNodeRemoved,
                this, idxToRemove, &m_nodes[idxToRemove]);

    m_nodes.RemoveByIndex(idxToRemove);

    for (AnimatorStateMachineNode& node : m_nodes)
    {
        for (uint i = 0; i < node.GetConnections().Size();)
        {
            AnimatorStateMachineConnection *conn = node.GetConnection(i);
            ASSERT(conn);

            if (conn->GetNodeToIndex() == i || conn->GetNodeFromIndex() == i)
            {
                // Must remove connection
                node.RemoveConnection(i);
            }
            else
            {
                if (conn->GetNodeToIndex() > i)
                {
                    conn->SetNodeToIndex( conn->GetNodeToIndex() - 1 );
                }
                if (conn->GetNodeFromIndex() > i)
                {
                    conn->SetNodeFromIndex( conn->GetNodeFromIndex() - 1 );
                }
                ++i;
            }
        }
    }

    if (idxToRemove == GetCurrentNodeIndex())
    {
        m_currentNodeIndex = -1u;
    }
}

void AnimatorStateMachine::Clear()
{
    m_currentNodeIndex = -1u;

    while (!m_nodes.IsEmpty())
    {
        RemoveNode( m_nodes.Size()-1 );
    }
}

const Array<AnimatorStateMachineNode>& AnimatorStateMachine::GetNodes() const
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

    for (const AnimatorStateMachineNode &smNode : GetNodes())
    {
        MetaNode smNodeMeta;
        smNode.ExportMeta(&smNodeMeta);
        metaNode->AddChild(smNodeMeta);
    }
}
