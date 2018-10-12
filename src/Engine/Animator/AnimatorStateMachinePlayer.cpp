#include "Bang/AnimatorStateMachinePlayer.h"

#include "Bang/AnimatorStateMachine.h"

USING_NAMESPACE_BANG

AnimatorStateMachinePlayer::AnimatorStateMachinePlayer()
{
}

AnimatorStateMachinePlayer::~AnimatorStateMachinePlayer()
{
}

void AnimatorStateMachinePlayer::SetStateMachine(AnimatorStateMachine *stateMachine)
{
    if (stateMachine != GetStateMachine())
    {
        if (GetStateMachine())
        {
            SetCurrentNode(nullptr);
            SetNextNode(nullptr);
            GetStateMachine()->EventEmitter<IEventsAnimatorStateMachine>::
                               UnRegisterListener(this);
        }

        p_stateMachine.Set(stateMachine);

        if (GetStateMachine())
        {
            GetStateMachine()->EventEmitter<IEventsAnimatorStateMachine>::
                               RegisterListener(this);
        }
    }
}

void AnimatorStateMachinePlayer::Step(Time deltaTime)
{
    AnimatorStateMachine *sm = GetStateMachine();
    if (!sm)
    {
        return;
    }

    if (!GetCurrentNode())
    {
        if (sm->GetNodes().Size() >= 1)
        {
            SetCurrentNode( sm->GetNodes().Front() );
        }
    }

    if (AnimatorStateMachineNode *currentNode = GetCurrentNode())
    {
        m_currentNodeTime += deltaTime;

        bool hasFinishedAnimation =
                (GetCurrentNodeTime().GetSeconds() >=
                 currentNode->GetAnimation()->GetDurationInSeconds());
        for (AnimatorStateMachineConnection *conn :
             currentNode->GetConnections())
        {
            if (hasFinishedAnimation || conn->GetImmediateTransition())
            {
                if (conn->AreTransitionConditionsFulfilled(sm))
                {
                    SetCurrentNode( conn->GetNodeTo() );
                    break;
                }
            }
        }
    }
}

void AnimatorStateMachinePlayer::SetCurrentNode(AnimatorStateMachineNode *node)
{
    if (node != GetCurrentNode())
    {
        if (GetCurrentNode())
        {
            GetCurrentNode()->EventEmitter<IEventsAnimatorStateMachineNode>::
                              UnRegisterListener(this);
        }

        p_currentNode = node;
        m_currentNodeTime = Time(0);

        if (GetCurrentNode())
        {
            GetCurrentNode()->EventEmitter<IEventsAnimatorStateMachineNode>::
                              RegisterListener(this);
        }
    }
}

void AnimatorStateMachinePlayer::SetNextNode(AnimatorStateMachineNode *node)
{
    if (node != GetNextNode())
    {
        if (GetNextNode())
        {
            GetNextNode()->EventEmitter<IEventsAnimatorStateMachineNode>::
                           UnRegisterListener(this);
        }

        p_nextNode = node;
        m_nextNodeTime = Time(0);

        if (GetNextNode())
        {
            GetNextNode()->EventEmitter<IEventsAnimatorStateMachineNode>::
                           RegisterListener(this);
        }
    }
}

AnimatorStateMachineNode *AnimatorStateMachinePlayer::GetCurrentNode() const
{
    return p_currentNode;
}

AnimatorStateMachineNode *AnimatorStateMachinePlayer::GetNextNode() const
{
    return p_nextNode;
}

Time AnimatorStateMachinePlayer::GetCurrentNodeTime() const
{
    return m_currentNodeTime;
}

Time AnimatorStateMachinePlayer::GetNextNodeTime() const
{
    return m_nextNodeTime;
}

AnimatorStateMachine *AnimatorStateMachinePlayer::GetStateMachine() const
{
    return p_stateMachine.Get();
}

void AnimatorStateMachinePlayer::OnNodeCreated(
                                    AnimatorStateMachine *stateMachine,
                                    uint newNodeIdx,
                                    AnimatorStateMachineNode *newNode)
{
    ASSERT(stateMachine == GetStateMachine());
    BANG_UNUSED_2(newNodeIdx, newNode);
}

void AnimatorStateMachinePlayer::OnNodeRemoved(
                                    AnimatorStateMachine *stateMachine,
                                    uint removedNodeIdx,
                                    AnimatorStateMachineNode *removedNode)
{
    ASSERT(stateMachine == GetStateMachine());
    if (removedNode == GetCurrentNode())
    {
        SetCurrentNode(nullptr);
    }
}

void AnimatorStateMachinePlayer::OnConnectionAdded(
                                    AnimatorStateMachineNode *node,
                                    AnimatorStateMachineConnection *connection)
{
    BANG_UNUSED_2(node, connection);
}

void AnimatorStateMachinePlayer::OnConnectionRemoved(
                                    AnimatorStateMachineNode *node,
                                    AnimatorStateMachineConnection *connection)
{
    BANG_UNUSED_2(node, connection);
}

