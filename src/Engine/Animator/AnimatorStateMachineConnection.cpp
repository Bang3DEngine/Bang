#include "Bang/AnimatorStateMachineConnection.h"

#include "Bang/AnimatorStateMachine.h"
#include "Bang/AnimatorStateMachineNode.h"
#include "Bang/DPtr.tcc"
#include "Bang/IEventsDestroy.h"
#include "Bang/MetaNode.h"
#include "Bang/MetaNode.tcc"
#include "Bang/StreamOperators.h"

using namespace Bang;

AnimatorStateMachineConnection::AnimatorStateMachineConnection(
    AnimatorStateMachine *stateMachine)
{
    p_stateMachine = stateMachine;
}

AnimatorStateMachineConnection::~AnimatorStateMachineConnection()
{
    EventEmitter<IEventsDestroy>::PropagateToListeners(
        &IEventsDestroy::OnDestroyed, this);
}

void AnimatorStateMachineConnection::SetNodeTo(AnimatorStateMachineNode *nodeTo)
{
    p_nodeTo = nodeTo;
}
void AnimatorStateMachineConnection::SetNodeFrom(
    AnimatorStateMachineNode *nodeFrom)
{
    p_nodeFrom = nodeFrom;
}

bool AnimatorStateMachineConnection::AreTransitionConditionsFulfilled(
    AnimatorStateMachine *animatorSM) const
{
    for(auto transCond : GetTransitionConditions())
    {
        if(!transCond->IsFulfilled(animatorSM))
        {
            return false;
        }
    }
    return true;
}

void AnimatorStateMachineConnection::SetTransitionDuration(
    Time transitionDuration)
{
    m_transitionDuration = transitionDuration;
}

void AnimatorStateMachineConnection::SetImmediateTransition(
    bool immediateTransition)
{
    m_immediateTransition = immediateTransition;
}

ASMCTransitionCondition *
AnimatorStateMachineConnection::CreateAndAddTransitionCondition()
{
    ASMCTransitionCondition *condition = new ASMCTransitionCondition();
    m_transitionConditions.PushBack(condition);
    return condition;
}

void AnimatorStateMachineConnection::RemoveTransitionCondition(
    ASMCTransitionCondition *transitionCond)
{
    RemoveTransitionCondition(m_transitionConditions.IndexOf(transitionCond));
}

void AnimatorStateMachineConnection::RemoveTransitionCondition(uint idx)
{
    if(idx < m_transitionConditions.Size())
    {
        ASMCTransitionCondition *transitionCond = m_transitionConditions[idx];
        m_transitionConditions.RemoveByIndex(idx);
        delete transitionCond;
    }
}

AnimatorStateMachineNode *AnimatorStateMachineConnection::GetNodeTo() const
{
    return p_nodeTo;
}

AnimatorStateMachineNode *AnimatorStateMachineConnection::GetNodeFrom() const
{
    return p_nodeFrom;
}

bool AnimatorStateMachineConnection::GetImmediateTransition() const
{
    return m_immediateTransition;
}

Time AnimatorStateMachineConnection::GetTransitionDuration() const
{
    return m_transitionDuration;
}

const Array<ASMCTransitionCondition *>
    &AnimatorStateMachineConnection::GetTransitionConditions() const
{
    return m_transitionConditions;
}

void AnimatorStateMachineConnection::CloneInto(
    AnimatorStateMachineConnection *cloneConnection) const
{
    cloneConnection->SetNodeFrom(GetNodeFrom());
    cloneConnection->SetNodeTo(GetNodeTo());
    cloneConnection->SetTransitionDuration(GetTransitionDuration());
    cloneConnection->SetImmediateTransition(GetImmediateTransition());
    cloneConnection->p_stateMachine = p_stateMachine;
}

void AnimatorStateMachineConnection::ImportMeta(const MetaNode &metaNode)
{
    Serializable::ImportMeta(metaNode);

    if(metaNode.Contains("NodeToIndex"))
    {
        uint idx = metaNode.Get<uint>("NodeToIndex");
        SetNodeTo(p_stateMachine->GetNodes()[idx]);
    }

    if(metaNode.Contains("NodeFromIndex"))
    {
        uint idx = metaNode.Get<uint>("NodeFromIndex");
        SetNodeFrom(p_stateMachine->GetNodes()[idx]);
    }

    if(metaNode.Contains("TransitionDuration"))
    {
        SetTransitionDuration(metaNode.Get<Time>("TransitionDuration"));
    }

    if(metaNode.Contains("ImmediateTransition"))
    {
        SetImmediateTransition(metaNode.Get<bool>("ImmediateTransition"));
    }

    const auto &transitionCondMetas =
        metaNode.GetChildren("TransitionConditions");
    for(const MetaNode &transCondMeta : transitionCondMetas)
    {
        ASMCTransitionCondition *transCond = CreateAndAddTransitionCondition();
        transCond->ImportMeta(transCondMeta);
    }
}

void AnimatorStateMachineConnection::ExportMeta(MetaNode *metaNode) const
{
    Serializable::ExportMeta(metaNode);

    metaNode->Set("NodeToIndex",
                  p_stateMachine->GetNodes().IndexOf(GetNodeTo()));
    metaNode->Set("ImmediateTransition", GetImmediateTransition());
    metaNode->Set("TransitionDuration", GetTransitionDuration());
    metaNode->Set("NodeFromIndex",
                  p_stateMachine->GetNodes().IndexOf(GetNodeFrom()));

    for(const AnimatorStateMachineConnectionTransitionCondition
            *transitionCond : GetTransitionConditions())
    {
        MetaNode transCondMeta = transitionCond->GetMeta();
        metaNode->AddChild(transCondMeta, "TransitionConditions");
    }
}
