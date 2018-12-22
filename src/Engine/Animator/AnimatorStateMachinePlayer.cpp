#include "Bang/AnimatorStateMachinePlayer.h"

#include "Bang/Animation.h"
#include "Bang/Animator.h"
#include "Bang/AnimatorStateMachine.h"
#include "Bang/AnimatorStateMachineLayer.h"
#include "Bang/AnimatorStateMachineNode.h"
#include "Bang/AnimatorStateMachineTransition.h"
#include "Bang/AnimatorStateMachineTransitionCondition.h"
#include "Bang/Array.h"
#include "Bang/Assert.h"
#include "Bang/EventEmitter.h"
#include "Bang/EventListener.tcc"
#include "Bang/IEventsAnimatorStateMachine.h"
#include "Bang/IEventsAnimatorStateMachineNode.h"

using namespace Bang;

AnimatorStateMachinePlayer::AnimatorStateMachinePlayer()
{
}

AnimatorStateMachinePlayer::~AnimatorStateMachinePlayer()
{
}

void AnimatorStateMachinePlayer::SetStateMachineLayer(
    AnimatorStateMachineLayer *stateMachineLayer)
{
    if (stateMachineLayer != GetStateMachineLayer())
    {
        if (GetStateMachineLayer())
        {
            p_currentTransition = nullptr;
            m_currentTransitionTime = Time(0);
            SetCurrentNode(nullptr);
            GetStateMachineLayer()
                ->EventEmitter<
                    IEventsAnimatorStateMachineLayer>::UnRegisterListener(this);
        }

        p_stateMachineLayer = stateMachineLayer;

        if (GetStateMachineLayer())
        {
            SetCurrentNode(GetStateMachineLayer()->GetEntryNodeOrFirstFound());
            GetStateMachineLayer()
                ->EventEmitter<
                    IEventsAnimatorStateMachineLayer>::RegisterListener(this);
        }
    }
}

void AnimatorStateMachinePlayer::Step(Animator *animator, Time deltaTime)
{
    AnimatorStateMachineLayer *smLayer = GetStateMachineLayer();
    if (!smLayer)
    {
        return;
    }

    if (!GetCurrentNode())
    {
        if (smLayer->GetNodes().Size() >= 1)
        {
            SetCurrentNode(smLayer->GetNodes().Front());
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
                FinishCurrentTransition(animator);
            }
        }

        {
            Time speededDeltaTime = deltaTime;
            speededDeltaTime *= GetCurrentNode()->GetSpeed();
            speededDeltaTime *= GetCurrentAnimation()
                                    ? GetCurrentAnimation()->GetSpeed()
                                    : 1.0f;
            m_currentNodeTime += speededDeltaTime;
        }

        // If we are NOT doing a transition, check if we can pick a new one
        if (!GetCurrentTransition())
        {
            if (GetCurrentNode()->GetAnimation())
            {
                bool hasFinishedAnimation =
                    (GetCurrentNodeTime().GetSeconds() >=
                     GetCurrentNode()->GetAnimation()->GetDurationInSeconds());
                for (AnimatorStateMachineTransition *conn :
                     GetCurrentNode()->GetTransitions())
                {
                    if (hasFinishedAnimation ||
                        !conn->GetWaitForAnimationToFinish())
                    {
                        if (conn->AreTransitionConditionsFulfilled(animator))
                        {
                            StartTransition(
                                conn, GetCurrentNodeTime(), Time(0));
                            break;
                        }
                    }
                }
            }
        }
    }
}

void AnimatorStateMachinePlayer::StartTransition(
    AnimatorStateMachineTransition *connection,
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

void AnimatorStateMachinePlayer::FinishCurrentTransition(Animator *animator)
{
    // Next node becomes the current node now
    if (GetCurrentTransition())
    {
        // Toggle off triggers
        for (auto *transCond :
             GetCurrentTransition()->GetTransitionConditions())
        {
            if (transCond->GetVariableType() ==
                AnimatorStateMachineVariable::Type::TRIGGER)
            {
                animator->SetVariableTrigger(transCond->GetVariableName(),
                                             false);
            }
        }
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
            GetCurrentNode()
                ->EventEmitter<
                    IEventsAnimatorStateMachineNode>::UnRegisterListener(this);
        }

        p_currentNode = node;
        m_currentNodeTime = nodeTime;

        if (GetCurrentNode())
        {
            GetCurrentNode()
                ->EventEmitter<
                    IEventsAnimatorStateMachineNode>::RegisterListener(this);
        }
    }
}

AnimatorStateMachineNode *AnimatorStateMachinePlayer::GetCurrentNode() const
{
    return p_currentNode;
}

AnimatorStateMachineNode *AnimatorStateMachinePlayer::GetNextNode() const
{
    return GetCurrentTransition() ? GetCurrentTransition()->GetNodeTo()
                                  : nullptr;
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

AnimatorStateMachineTransition *
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
    return GetCurrentTransition()
               ? GetCurrentTransition()->GetTransitionDuration()
               : Time(0);
}

AnimatorStateMachine *AnimatorStateMachinePlayer::GetStateMachine() const
{
    return GetStateMachineLayer() ? GetStateMachineLayer()->GetStateMachine()
                                  : nullptr;
}

AnimatorStateMachineLayer *AnimatorStateMachinePlayer::GetStateMachineLayer()
    const
{
    return p_stateMachineLayer;
}

void AnimatorStateMachinePlayer::OnNodeCreated(
    uint newNodeIdx,
    AnimatorStateMachineNode *newNode)
{
    BANG_UNUSED_2(newNodeIdx, newNode);
}

void AnimatorStateMachinePlayer::OnNodeRemoved(
    uint removedNodeIdx,
    AnimatorStateMachineNode *removedNode)
{
    BANG_UNUSED(removedNodeIdx);
    if (removedNode == GetCurrentNode())
    {
        SetCurrentNode(nullptr);
    }
}

void AnimatorStateMachinePlayer::OnTransitionAdded(
    AnimatorStateMachineNode *node,
    AnimatorStateMachineTransition *transition)
{
    BANG_UNUSED_2(node, transition);
}

void AnimatorStateMachinePlayer::OnTransitionRemoved(
    AnimatorStateMachineNode *node,
    AnimatorStateMachineTransition *transition)
{
    BANG_UNUSED_2(node, transition);
}
