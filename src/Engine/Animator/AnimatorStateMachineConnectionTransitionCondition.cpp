#include "Bang/AnimatorStateMachineConnectionTransitionCondition.h"

#include "Bang/AnimatorStateMachine.h"
#include "Bang/AnimatorStateMachineVariable.h"
#include "Bang/Assert.h"
#include "Bang/MetaNode.h"
#include "Bang/MetaNode.tcc"

using namespace Bang;

AnimatorStateMachineConnectionTransitionCondition::
    AnimatorStateMachineConnectionTransitionCondition()
{
}

AnimatorStateMachineConnectionTransitionCondition::
    ~AnimatorStateMachineConnectionTransitionCondition()
{
}

void ASMCTransitionCondition::SetVariableName(const String &variableName)
{
    m_varName = variableName;
}

void ASMCTransitionCondition::SetVariableType(
    AnimatorStateMachineVariable::Type type)
{
    m_varType = type;
}

void ASMCTransitionCondition::SetComparator(
    ASMCTransitionCondition::Comparator comparator)
{
    m_comparator = comparator;
}

void ASMCTransitionCondition::SetCompareValueFloat(float compareValueFloat)
{
    m_compareValueFloat = compareValueFloat;
}

const String &ASMCTransitionCondition::GetVariableName() const
{
    return m_varName;
}

AnimatorStateMachineVariable::Type ASMCTransitionCondition::GetVariableType()
    const
{
    return m_varType;
}

ASMCTransitionCondition::Comparator ASMCTransitionCondition::GetComparator()
    const
{
    return m_comparator;
}

float ASMCTransitionCondition::GetCompareValueFloat() const
{
    return m_compareValueFloat;
}

bool ASMCTransitionCondition::IsFulfilled(
    AnimatorStateMachine *stateMachine) const
{
    if (stateMachine)
    {
        if (AnimatorStateMachineVariable *var =
                stateMachine->GetVariable(GetVariableName()))
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

void AnimatorStateMachineConnectionTransitionCondition::ImportMeta(
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

void AnimatorStateMachineConnectionTransitionCondition::ExportMeta(
    MetaNode *metaNode) const
{
    Serializable::ExportMeta(metaNode);

    metaNode->Set("VariableName", GetVariableName());
    metaNode->Set("VariableType", m_varType);
    metaNode->Set("Comparator", GetComparator());
    metaNode->Set("CompareValueFloat", GetCompareValueFloat());
}
