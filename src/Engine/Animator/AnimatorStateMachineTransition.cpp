#include "Bang/AnimatorStateMachineTransition.h"

#include "Bang/AnimatorStateMachine.h"
#include "Bang/AnimatorStateMachineLayer.h"
#include "Bang/AnimatorStateMachineNode.h"
#include "Bang/AnimatorStateMachineTransitionCondition.h"
#include "Bang/DPtr.tcc"
#include "Bang/IEventsDestroy.h"
#include "Bang/MetaNode.h"
#include "Bang/MetaNode.tcc"
#include "Bang/StreamOperators.h"

using namespace Bang;

AnimatorStateMachineTransition::AnimatorStateMachineTransition()
{
}

AnimatorStateMachineTransition::~AnimatorStateMachineTransition()
{
    EventEmitter<IEventsDestroy>::PropagateToListeners(
        &IEventsDestroy::OnDestroyed, this);
}

void AnimatorStateMachineTransition::SetNodeTo(AnimatorStateMachineNode *nodeTo)
{
    p_nodeTo = nodeTo;
}
void AnimatorStateMachineTransition::SetNodeFrom(
    AnimatorStateMachineNode *nodeFrom)
{
    p_nodeFrom = nodeFrom;
}

bool AnimatorStateMachineTransition::AreTransitionConditionsFulfilled(
    Animator *animator) const
{
    for (auto transCond : GetTransitionConditions())
    {
        if (!transCond->IsFulfilled(animator))
        {
            return false;
        }
    }
    return true;
}

void AnimatorStateMachineTransition::SetTransitionDuration(
    Time transitionDuration)
{
    m_transitionDuration = transitionDuration;
}

void AnimatorStateMachineTransition::SetWaitForAnimationToFinish(
    bool waitForAnimationToFinish)
{
    m_waitForAnimationToFinish = waitForAnimationToFinish;
}

AnimatorStateMachineTransitionCondition *
AnimatorStateMachineTransition::CreateAndAddTransitionCondition()
{
    AnimatorStateMachineTransitionCondition *condition =
        new AnimatorStateMachineTransitionCondition();
    condition->SetTransition(this);
    m_transitionConditions.PushBack(condition);
    return condition;
}

void AnimatorStateMachineTransition::RemoveTransitionCondition(
    AnimatorStateMachineTransitionCondition *transitionCond)
{
    RemoveTransitionCondition(m_transitionConditions.IndexOf(transitionCond));
}

void AnimatorStateMachineTransition::RemoveTransitionCondition(uint idx)
{
    if (idx < m_transitionConditions.Size())
    {
        AnimatorStateMachineTransitionCondition *transitionCond =
            m_transitionConditions[idx];
        m_transitionConditions.RemoveByIndex(idx);
        delete transitionCond;
    }
}

AnimatorStateMachineNode *AnimatorStateMachineTransition::GetNodeTo() const
{
    return p_nodeTo;
}

AnimatorStateMachineNode *AnimatorStateMachineTransition::GetNodeFrom() const
{
    return p_nodeFrom;
}

bool AnimatorStateMachineTransition::GetWaitForAnimationToFinish() const
{
    return m_waitForAnimationToFinish;
}

Time AnimatorStateMachineTransition::GetTransitionDuration() const
{
    return m_transitionDuration;
}

AnimatorStateMachine *AnimatorStateMachineTransition::GetStateMachine() const
{
    return GetLayer() ? GetLayer()->GetStateMachine() : nullptr;
}

AnimatorStateMachineLayer *AnimatorStateMachineTransition::GetLayer() const
{
    return GetNodeFrom() ? GetNodeFrom()->GetLayer() : nullptr;
}

const Array<AnimatorStateMachineTransitionCondition *>
    &AnimatorStateMachineTransition::GetTransitionConditions() const
{
    return m_transitionConditions;
}

void AnimatorStateMachineTransition::CloneInto(ICloneable *clone,
                                               bool cloneGUID) const
{
    Serializable::CloneInto(clone, cloneGUID);

    AnimatorStateMachineTransition *cloneTransition =
        SCAST<AnimatorStateMachineTransition *>(clone);
    cloneTransition->SetNodeFrom(GetNodeFrom());
    cloneTransition->SetNodeTo(GetNodeTo());
    cloneTransition->SetTransitionDuration(GetTransitionDuration());
    cloneTransition->SetWaitForAnimationToFinish(GetWaitForAnimationToFinish());
}

void AnimatorStateMachineTransition::ImportMeta(const MetaNode &metaNode)
{
    Serializable::ImportMeta(metaNode);

    if (GetLayer())
    {
        if (metaNode.Contains("NodeToIndex"))
        {
            uint idx = metaNode.Get<uint>("NodeToIndex");
            SetNodeTo(GetLayer()->GetNodes()[idx]);
        }

        if (metaNode.Contains("NodeFromIndex"))
        {
            uint idx = metaNode.Get<uint>("NodeFromIndex");
            SetNodeFrom(GetLayer()->GetNodes()[idx]);
        }
    }

    if (metaNode.Contains("TransitionDuration"))
    {
        SetTransitionDuration(metaNode.Get<Time>("TransitionDuration"));
    }

    if (metaNode.Contains("WaitForAnimationToFinish"))
    {
        SetWaitForAnimationToFinish(
            metaNode.Get<bool>("WaitForAnimationToFinish"));
    }

    const auto &transitionCondMetas =
        metaNode.GetChildren("TransitionConditions");
    for (const MetaNode &transCondMeta : transitionCondMetas)
    {
        AnimatorStateMachineTransitionCondition *transCond =
            CreateAndAddTransitionCondition();
        transCond->ImportMeta(transCondMeta);
    }
}

void AnimatorStateMachineTransition::ExportMeta(MetaNode *metaNode) const
{
    Serializable::ExportMeta(metaNode);

    if (GetLayer())
    {
        metaNode->Set("NodeToIndex",
                      GetLayer()->GetNodes().IndexOf(GetNodeTo()));
        metaNode->Set("NodeFromIndex",
                      GetLayer()->GetNodes().IndexOf(GetNodeFrom()));
    }

    metaNode->Set("WaitForAnimationToFinish", GetWaitForAnimationToFinish());
    metaNode->Set("TransitionDuration", GetTransitionDuration());

    metaNode->CreateChildrenContainer("TransitionConditions");
    for (const AnimatorStateMachineTransitionCondition *transitionCond :
         GetTransitionConditions())
    {
        MetaNode transCondMeta = transitionCond->GetMeta();
        metaNode->AddChild(transCondMeta, "TransitionConditions");
    }
}
