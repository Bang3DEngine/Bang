#include "Bang/AnimatorStateMachineTransitionCondition.h"

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

void AnimatorStateMachineTransitionCondition::SetVariableType(
    AnimatorStateMachineVariable::Type type)
{
    m_varType = type;
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
    return m_varType;
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

AnimatorStateMachineTransition *
AnimatorStateMachineTransitionCondition::GetTransition() const
{
    return p_transition;
}

bool AnimatorStateMachineTransitionCondition::IsFulfilled() const
{
    if (GetStateMachine())
    {
        if (AnimatorStateMachineVariable *var =
                GetStateMachine()->GetVariable(GetVariableName()))
        {
            switch (var->GetType())
            {
                case AnimatorStateMachineVariable::Type::FLOAT:
                    switch (GetComparator())
                    {
                        case Comparator::GREATER:
                            return (var->GetValueFloat() >
                                    GetCompareValueFloat());

                        case Comparator::LESS:
                            return (var->GetValueFloat() <
                                    GetCompareValueFloat());

                        default: ASSERT(false); break;
                    }
                    break;

                case AnimatorStateMachineVariable::Type::BOOL:
                    switch (GetComparator())
                    {
                        case Comparator::IS_TRUE: return (var->GetValueBool());
                        case Comparator::IS_FALSE:
                            return (!var->GetValueBool());
                        default: ASSERT(false); break;
                    }
                    break;
            }
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
    metaNode->Set("VariableType", m_varType);
    metaNode->Set("Comparator", GetComparator());
    metaNode->Set("CompareValueFloat", GetCompareValueFloat());
}
