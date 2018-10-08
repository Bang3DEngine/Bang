#include "Bang/AnimatorStateMachineConnection.h"

#include "Bang/AnimatorStateMachine.h"
#include "Bang/AnimatorStateMachineNode.h"

USING_NAMESPACE_BANG

AnimatorStateMachineConnection::AnimatorStateMachineConnection()
{
}

AnimatorStateMachineConnection::~AnimatorStateMachineConnection()
{
}

void AnimatorStateMachineConnection::SetNodeToIndex(uint nodeToIdx)
{
    m_nodeToIndex = nodeToIdx;
}
void AnimatorStateMachineConnection::SetNodeFromIndex(uint nodeFromIdx)
{
    m_nodeFromIndex = nodeFromIdx;
}

void AnimatorStateMachineConnection::SetNodeTo(AnimatorStateMachineNode *nodeTo)
{
    SetNodeToIndex( GetSMNodeIdx(nodeTo) );
}
void AnimatorStateMachineConnection::SetNodeFrom(AnimatorStateMachineNode *nodeFrom)
{
    SetNodeFromIndex( GetSMNodeIdx(nodeFrom) );
}

uint AnimatorStateMachineConnection::GetNodeToIndex() const
{
    return m_nodeToIndex;
}

uint AnimatorStateMachineConnection::GetNodeFromIndex() const
{
    return m_nodeFromIndex;
}

AnimatorStateMachineNode *AnimatorStateMachineConnection::GetNodeTo() const
{
    return GetSMNode(GetNodeToIndex());
}

AnimatorStateMachineNode *AnimatorStateMachineConnection::GetNodeFrom() const
{
    return GetSMNode(GetNodeFromIndex());
}

AnimatorStateMachine *AnimatorStateMachineConnection::GetStateMachine() const
{
    return p_stateMachine;
}

uint AnimatorStateMachineConnection::GetIndexInsideNodeConnections(
                                                uint nodeIdx) const
{
    if (auto smNode = GetSMNode(nodeIdx))
    {
        for (uint i = 0; i < smNode->GetConnections().Size(); ++i)
        {
            if (smNode->GetConnection(i) == this)
            {
                return i;
            }
        }
    }
    return -1u;
}

void AnimatorStateMachineConnection::CloneInto(
                    AnimatorStateMachineConnection *cloneConnection) const
{
    cloneConnection->SetNodeFromIndex( GetNodeFromIndex() );
    cloneConnection->SetNodeToIndex( GetNodeToIndex() );
    cloneConnection->SetStateMachine( GetStateMachine() );
}

AnimatorStateMachineNode *AnimatorStateMachineConnection::GetSMNode(uint idx) const
{
    if (GetStateMachine())
    {
        return GetStateMachine()->GetNode(idx);
    }
    return nullptr;
}

uint AnimatorStateMachineConnection::GetSMNodeIdx(
                                const AnimatorStateMachineNode *node) const
{
    for (uint i = 0; i < GetStateMachine()->GetNodes().Size(); ++i)
    {
        if (node == &(GetStateMachine()->GetNodes()[i]))
        {
            return i;
        }
    }
    return -1u;
}

void AnimatorStateMachineConnection::SetStateMachine(
                                            AnimatorStateMachine *stateMachine)
{
    p_stateMachine = stateMachine;
}
