#include "Bang/AnimatorStateMachineTransitionCondition.h"

#include "Bang/Animator.h"
#include "Bang/AnimatorStateMachine.h"
#include "Bang/AnimatorStateMachineLayer.h"
#include "Bang/AnimatorStateMachineTransition.h"
#include "Bang/AnimatorStateMachineVariable.h"
#include "Bang/Assert.h"
#include "Bang/MetaNode.h"
#include "Bang/MetaNode.tcc"

using namespace Bang;

AnimatorStateMachineTransitionCondition::
    AnimatorStateMachineTransitionCondition()
{
    EventEmitter<IEventsDestroy>::PropagateToListeners(
        &IEventsDestroy::OnDestroyed, this);
}

AnimatorStateMachineTransitionCondition::
    ~AnimatorStateMachineTransitionCondition()
{
}

void AnimatorStateMachineTransitionCondition::SetTransition(
    AnimatorStateMachineTransition *transition)
{
    p_transition = transition;
}

void AnimatorStateMachineTransitionCondition::SetVariableName(
    const String &variableName)
{
    m_varName = variableName;
}

void AnimatorStateMachineTransitionCondition::SetComparator(
    AnimatorStateMachineTransitionCondition::Comparator comparator)
{
    m_comparator = comparator;
}

void AnimatorStateMachineTransitionCondition::SetCompareValueFloat(
    float compareValueFloat)
{
    m_compareValueFloat = compareValueFloat;
}

const String &AnimatorStateMachineTransitionCondition::GetVariableName() const
{
    return m_varName;
}

AnimatorStateMachineVariable::Type
AnimatorStateMachineTransitionCondition::GetVariableType() const
{
    if (AnimatorStateMachineVariable *var = GetVariable())
    {
        return var->GetType();
    }
    return AnimatorStateMachineVariable::Type::FLOAT;
}

AnimatorStateMachineTransitionCondition::Comparator
AnimatorStateMachineTransitionCondition::GetComparator() const
{
    return m_comparator;
}

float AnimatorStateMachineTransitionCondition::GetCompareValueFloat() const
{
    return m_compareValueFloat;
}

AnimatorStateMachine *AnimatorStateMachineTransitionCondition::GetStateMachine()
    const
{
    return GetLayer() ? GetLayer()->GetStateMachine() : nullptr;
}

AnimatorStateMachineLayer *AnimatorStateMachineTransitionCondition::GetLayer()
    const
{
    return GetTransition() ? GetTransition()->GetLayer() : nullptr;
}

AnimatorStateMachineVariable *
AnimatorStateMachineTransitionCondition::GetVariable() const
{
    return GetStateMachine()
               ? GetStateMachine()->GetVariableDefault(GetVariableName())
               : nullptr;
}

AnimatorStateMachineTransition *
AnimatorStateMachineTransitionCondition::GetTransition() const
{
    return p_transition;
}

bool AnimatorStateMachineTransitionCondition::IsFulfilled(
    Animator *animator) const
{
    if (GetStateMachine())
    {
        const Variant var = animator->GetVariableVariant(GetVariableName());
        switch (animator->GetVariableType(GetVariableName()))
        {
            case AnimatorStateMachineVariable::Type::FLOAT:
                switch (GetComparator())
                {
                    case Comparator::GREATER:
                        return (var.GetFloat() > GetCompareValueFloat());

                    case Comparator::LESS:
                        return (var.GetFloat() < GetCompareValueFloat());

                    default: break;
                }
                break;

            case AnimatorStateMachineVariable::Type::BOOL:
            case AnimatorStateMachineVariable::Type::TRIGGER:
                switch (GetComparator())
                {
                    case Comparator::IS_TRUE: return (var.GetBool());
                    case Comparator::IS_FALSE: return (!var.GetBool());
                    default: break;
                }
                break;
            default: ASSERT(false); return false;
        }
    }
    return false;
}

void AnimatorStateMachineTransitionCondition::ImportMeta(
    const MetaNode &metaNode)
{
    Serializable::ImportMeta(metaNode);

    if (metaNode.Contains("VariableName"))
    {
        SetVariableName(metaNode.Get<String>("VariableName"));
    }

    if (metaNode.Contains("Comparator"))
    {
        SetComparator(SCAST<Comparator>(metaNode.Get<uint>("Comparator")));
    }

    if (metaNode.Contains("CompareValueFloat"))
    {
        SetCompareValueFloat(metaNode.Get<float>("CompareValueFloat"));
    }
}

void AnimatorStateMachineTransitionCondition::ExportMeta(
    MetaNode *metaNode) const
{
    Serializable::ExportMeta(metaNode);

    metaNode->Set("VariableName", GetVariableName());
    metaNode->Set("Comparator", GetComparator());
    metaNode->Set("CompareValueFloat", GetCompareValueFloat());
}
