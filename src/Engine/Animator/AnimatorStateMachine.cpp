#include "Bang/AnimatorStateMachine.h"

#include "Bang/AnimatorStateMachineNode.h"
#include "Bang/AnimatorStateMachineNode.h"

USING_NAMESPACE_BANG

AnimatorStateMachine::AnimatorStateMachine()
{
    CreateNodeInto(&m_entryNode);
}

AnimatorStateMachine::~AnimatorStateMachine()
{
}

void AnimatorStateMachine::SetAnimator(Animator *animator)
{
    p_animator = animator;
}

void AnimatorStateMachine::CreateNodeInto(AnimatorStateMachineNode *node)
{
    node->SetAnimatorStateMachine(this);
}

AnimatorStateMachineNode *AnimatorStateMachine::CreateNode()
{
    AnimatorStateMachineNode smNode;
    CreateNodeInto(&smNode);
    m_nodes.PushBack(smNode);
    return &m_nodes.Back();
}

AnimatorStateMachineNode *AnimatorStateMachine::GetEntryNode()
{
    return &m_entryNode;
}
const AnimatorStateMachineNode *AnimatorStateMachine::GetEntryNode() const
{
    return &m_entryNode;
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

void AnimatorStateMachine::RemoveNode(AnimatorStateMachineNode *node)
{
    for (uint i = 0; i < m_nodes.Size(); ++i)
    {
        if (node == &m_nodes[i])
        {
            RemoveNode(i);
            break;
        }
    }
}

void AnimatorStateMachine::RemoveNode(uint idxToRemove)
{
    ASSERT(idxToRemove < m_nodes.Size());
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
}

const Array<AnimatorStateMachineNode>& AnimatorStateMachine::GetNodes() const
{
    return m_nodes;
}

void AnimatorStateMachine::Import(const Path &resourceFilepath)
{
    BANG_UNUSED(resourceFilepath);
}

void AnimatorStateMachine::ImportMeta(const MetaNode &metaNode)
{
    Resource::ImportMeta(metaNode);
}

void AnimatorStateMachine::ExportMeta(MetaNode *metaNode) const
{
    Resource::ExportMeta(metaNode);
}
