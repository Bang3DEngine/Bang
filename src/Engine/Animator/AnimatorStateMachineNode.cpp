#include "Bang/AnimatorStateMachineNode.h"

#include "Bang/Animation.h"
#include "Bang/Animator.h"
#include "Bang/AnimatorStateMachine.h"
#include "Bang/AnimatorStateMachineLayer.h"
#include "Bang/AnimatorStateMachineTransition.h"
#include "Bang/Assert.h"
#include "Bang/Assets.h"
#include "Bang/Assets.tcc"
#include "Bang/DPtr.tcc"
#include "Bang/GUID.h"
#include "Bang/IEventsAnimatorStateMachineNode.h"
#include "Bang/IEventsDestroy.h"
#include "Bang/MetaNode.h"
#include "Bang/MetaNode.tcc"

using namespace Bang;

AnimatorStateMachineNode::AnimatorStateMachineNode()
{
    SetName("Node");
}

AnimatorStateMachineNode::~AnimatorStateMachineNode()
{
    EventEmitter<IEventsDestroy>::PropagateToListeners(
        &IEventsDestroy::OnDestroyed, this);

    while (!m_transitions.IsEmpty())
    {
        RemoveTransition(m_transitions.Back());
    }
}

void AnimatorStateMachineNode::SetName(const String &name)
{
    m_name = name;
}

AnimatorStateMachineTransition *AnimatorStateMachineNode::CreateTransitionTo(
    AnimatorStateMachineNode *nodeTo)
{
    auto newTransition = new AnimatorStateMachineTransition();
    newTransition->SetNodeFrom(this);
    newTransition->SetNodeTo(nodeTo);
    return AddTransition(newTransition);
}

const AnimatorStateMachineTransition *AnimatorStateMachineNode::GetTransition(
    uint transitionIdx) const
{
    return const_cast<AnimatorStateMachineNode *>(this)->GetTransition(
        transitionIdx);
}

AnimatorStateMachineTransition *AnimatorStateMachineNode::GetTransition(
    uint transitionIdx)
{
    if (transitionIdx < GetTransitions().Size())
    {
        return m_transitions[transitionIdx];
    }
    return nullptr;
}

void AnimatorStateMachineNode::RemoveTransition(
    AnimatorStateMachineTransition *transitionToRemove)
{
    if (m_transitions.Contains(transitionToRemove))
    {
        m_transitions.Remove(transitionToRemove);

        EventEmitter<IEventsAnimatorStateMachineNode>::PropagateToListeners(
            &IEventsAnimatorStateMachineNode::OnTransitionRemoved,
            this,
            transitionToRemove);

        delete transitionToRemove;
    }
}

Map<String, Transformation> AnimatorStateMachineNode::GetBoneTransformations(
    Time animationTime,
    Animator *) const
{
    Map<String, Transformation> bonesTransformations =
        Animation::GetBoneAnimationTransformations(GetAnimation(),
                                                   animationTime);
    return bonesTransformations;
}

void AnimatorStateMachineNode::SetSpeed(float speed)
{
    m_speed = speed;
}

void AnimatorStateMachineNode::SetLayer(
    AnimatorStateMachineLayer *stateMachineLayer)
{
    p_layer = stateMachineLayer;
}

AnimatorStateMachineTransition *AnimatorStateMachineNode::AddTransition(
    AnimatorStateMachineTransition *newTransition)
{
    ASSERT(newTransition->GetNodeFrom() == this);
    ASSERT(newTransition->GetNodeTo() != nullptr);
    ASSERT(newTransition->GetNodeTo() != this);

    m_transitions.PushBack(newTransition);
    EventEmitter<IEventsAnimatorStateMachineNode>::PropagateToListeners(
        &IEventsAnimatorStateMachineNode::OnTransitionAdded,
        this,
        newTransition);

    return newTransition;
}

void AnimatorStateMachineNode::SetAnimation(Animation *animation)
{
    p_animation.Set(animation);
}

const String &AnimatorStateMachineNode::GetName() const
{
    return m_name;
}

Animation *AnimatorStateMachineNode::GetAnimation() const
{
    return p_animation.Get();
}

Array<AnimatorStateMachineTransition *>
AnimatorStateMachineNode::GetTransitionsTo(
    AnimatorStateMachineNode *nodeTo) const
{
    Array<AnimatorStateMachineTransition *> transitionsToNode;
    for (AnimatorStateMachineTransition *transition : GetTransitions())
    {
        if (transition->GetNodeTo() == nodeTo)
        {
            transitionsToNode.PushBack(transition);
        }
    }
    return transitionsToNode;
}

const Array<AnimatorStateMachineTransition *>
    &AnimatorStateMachineNode::GetTransitions() const
{
    return m_transitions;
}

float AnimatorStateMachineNode::GetSpeed() const
{
    return m_speed;
}

AnimatorStateMachine *AnimatorStateMachineNode::GetStateMachine() const
{
    return GetLayer() ? GetLayer()->GetStateMachine() : nullptr;
}

AnimatorStateMachineLayer *AnimatorStateMachineNode::GetLayer() const
{
    return p_layer;
}

void AnimatorStateMachineNode::CloneInto(ICloneable *clone,
                                         bool cloneGUID) const
{
    Serializable::CloneInto(clone, cloneGUID);

    AnimatorStateMachineNode *nodeClone =
        SCAST<AnimatorStateMachineNode *>(clone);
    nodeClone->SetName(GetName());
    nodeClone->SetSpeed(GetSpeed());
    nodeClone->SetAnimation(GetAnimation());
}

void AnimatorStateMachineNode::ImportMeta(const MetaNode &metaNode)
{
    Serializable::ImportMeta(metaNode);

    if (metaNode.Contains("NodeName"))
    {
        SetName(metaNode.Get<String>("NodeName"));
    }

    if (metaNode.Contains("Animation"))
    {
        SetAnimation(
            Assets::Load<Animation>(metaNode.Get<GUID>("Animation")).Get());
    }

    if (metaNode.Contains("Speed"))
    {
        SetSpeed(metaNode.Get<float>("Speed"));
    }

    for (const MetaNode &childMetaNode : metaNode.GetChildren("Transitions"))
    {
        auto newTransition = new AnimatorStateMachineTransition();
        newTransition->SetNodeFrom(this);
        newTransition->ImportMeta(childMetaNode);
        AddTransition(newTransition);
    }
}

void AnimatorStateMachineNode::ExportMeta(MetaNode *metaNode) const
{
    Serializable::ExportMeta(metaNode);

    metaNode->Set("NodeName", GetName());
    metaNode->Set("Animation",
                  GetAnimation() ? GetAnimation()->GetGUID() : GUID::Empty());
    metaNode->Set("Speed", GetSpeed());

    metaNode->CreateChildrenContainer("Transitions");
    for (const AnimatorStateMachineTransition *smTransition : GetTransitions())
    {
        MetaNode smTransitionMeta = smTransition->GetMeta();
        metaNode->AddChild(smTransitionMeta, "Transitions");
    }
}
