#include "Bang/AnimatorStateMachinePlayer.h"

#include "Bang/Animation.h"
#include "Bang/AnimatorStateMachine.h"
#include "Bang/AnimatorStateMachineConnection.h"
#include "Bang/AnimatorStateMachineNode.h"
#include "Bang/Array.h"
#include "Bang/Assert.h"
#include "Bang/EventEmitter.h"
#include "Bang/EventListener.tcc"
#include "Bang/IEventsAnimatorStateMachine.h"
#include "Bang/IEventsAnimatorStateMachineNode.h"

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
            p_currentTransition = nullptr;
            m_currentTransitionTime = Time(0);
            SetCurrentNode(nullptr);
            GetStateMachine()->EventEmitter<IEventsAnimatorStateMachine>::
                               UnRegisterListener(this);
        }

        p_stateMachine.Set(stateMachine);

        if (GetStateMachine())
        {
            SetCurrentNode( GetStateMachine()->GetEntryNodeOrFirstFound() );
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

    if (GetCurrentNode())
    {
        if (GetCurrentTransition())
        {
            ASSERT(GetNextNode());
            m_currentTransitionTime += deltaTime;
            if (GetCurrentTransitionTime() >= GetCurrentTransitionDuration())
            {
                FinishCurrentTransition();
            }
        }

        m_currentNodeTime += deltaTime;

        // If we are NOT doing a transition, check if we can pick one of them
        if (!GetCurrentTransition())
        {
            bool hasFinishedAnimation =
                    (GetCurrentNodeTime().GetSeconds() >=
                     GetCurrentNode()->GetAnimation()->GetDurationInSeconds());
            for (AnimatorStateMachineConnection *conn :
                 GetCurrentNode()->GetConnections())
            {
                if (hasFinishedAnimation || conn->GetImmediateTransition())
                {
                    if (conn->AreTransitionConditionsFulfilled(sm))
                    {
                        StartTransition(conn, GetCurrentNodeTime(), Time(0));
                        break;
                    }
                }
            }
        }
    }
}

void AnimatorStateMachinePlayer::StartTransition(
                            AnimatorStateMachineConnection *connection,
                            Time prevNodeTime,
                            Time startTransitionTime)
{
    AnimatorStateMachineNode *prevNode = connection->GetNodeFrom();
    ASSERT(prevNode);

    AnimatorStateMachineNode *nextNode = connection->GetNodeTo();
    ASSERT(nextNode);

    p_currentTransition = connection;
    m_currentTransitionTime = startTransitionTime;

    SetCurrentNode(prevNode, prevNodeTime);
}

void AnimatorStateMachinePlayer::FinishCurrentTransition()
{
    // Next node becomes the current node now
    if (GetCurrentTransition())
    {
        SetCurrentNode(GetNextNode(), GetNextNodeTime());

        p_currentTransition = nullptr;
        m_currentTransitionTime = Time(0);
    }
}

void AnimatorStateMachinePlayer::SetCurrentNode(AnimatorStateMachineNode *node)
{
    SetCurrentNode(node, Time(0));
}

void AnimatorStateMachinePlayer::SetCurrentNode(AnimatorStateMachineNode *node,
                                                Time nodeTime)
{
    if (node != GetCurrentNode())
    {
        if (GetCurrentNode())
        {
            GetCurrentNode()->EventEmitter<IEventsAnimatorStateMachineNode>::
                              UnRegisterListener(this);
        }

        p_currentNode = node;
        m_currentNodeTime = nodeTime;

        if (GetCurrentNode())
        {
            GetCurrentNode()->EventEmitter<IEventsAnimatorStateMachineNode>::
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
    return GetCurrentTransition() ? GetCurrentTransition()->GetNodeTo() :
                                    nullptr;
}

Animation *AnimatorStateMachinePlayer::GetCurrentAnimation() const
{
    return (GetCurrentNode() ? GetCurrentNode()->GetAnimation() : nullptr);
}

Animation *AnimatorStateMachinePlayer::GetNextAnimation() const
{
    return (GetNextNode() ? GetNextNode()->GetAnimation() : nullptr);
}

Time AnimatorStateMachinePlayer::GetCurrentNodeTime() const
{
    return m_currentNodeTime;
}

Time AnimatorStateMachinePlayer::GetNextNodeTime() const
{
    return m_currentTransitionTime;
}

AnimatorStateMachineConnection*
AnimatorStateMachinePlayer::GetCurrentTransition() const
{
    return p_currentTransition;
}

Time AnimatorStateMachinePlayer::GetCurrentTransitionTime() const
{
    return m_currentTransitionTime;
}

Time AnimatorStateMachinePlayer::GetCurrentTransitionDuration() const
{
    return GetCurrentTransition() ?
                GetCurrentTransition()->GetTransitionDuration() : Time(0);
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
    BANG_UNUSED(removedNodeIdx);
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

