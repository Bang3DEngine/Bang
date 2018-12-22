#include "Bang/AnimatorStateMachineVariable.h"

#include <vector>

#include "Bang/AnimatorStateMachine.h"
#include "Bang/Array.tcc"
#include "Bang/EventEmitter.h"
#include "Bang/EventEmitter.tcc"
#include "Bang/EventListener.tcc"
#include "Bang/IEventsAnimatorStateMachine.h"
#include "Bang/MetaNode.h"
#include "Bang/MetaNode.tcc"

using namespace Bang;

AnimatorStateMachineVariable::AnimatorStateMachineVariable()
{
}

AnimatorStateMachineVariable::~AnimatorStateMachineVariable()
{
}

void AnimatorStateMachineVariable::SetType(Variant::Type type)
{
    m_variant.SetType(type);
}

void AnimatorStateMachineVariable::SetValueFloat(float value)
{
    m_variant.SetFloat(value);
}

void AnimatorStateMachineVariable::SetValueBool(bool value)
{
    m_variant.SetBool(value);
}

void AnimatorStateMachineVariable::SetVariant(const Variant &variant)
{
    m_variant = variant;
}

const String &AnimatorStateMachineVariable::GetName() const
{
    return m_name;
}

Variant::Type AnimatorStateMachineVariable::GetType() const
{
    return GetVariant().GetType();
}

bool AnimatorStateMachineVariable::GetValueBool() const
{
    return GetVariant().GetBool();
}

float AnimatorStateMachineVariable::GetValueFloat() const
{
    return GetVariant().GetFloat();
}

const Variant &AnimatorStateMachineVariable::GetVariant() const
{
    return m_variant;
}

AnimatorStateMachine *AnimatorStateMachineVariable::GetStateMachine() const
{
    return p_stateMachine;
}

void AnimatorStateMachineVariable::SetStateMachine(
    AnimatorStateMachine *stateMachine)
{
    p_stateMachine = stateMachine;
}

void AnimatorStateMachineVariable::SetName(const String &varName)
{
    String prevName = GetName();

    m_name = varName;

    p_stateMachine
        ->EventEmitter<IEventsAnimatorStateMachine>::PropagateToListeners(
            &IEventsAnimatorStateMachine::OnVariableNameChanged,
            this,
            prevName,
            varName);
}

void AnimatorStateMachineVariable::ImportMeta(const MetaNode &metaNode)
{
    Serializable::ImportMeta(metaNode);

    if (metaNode.Contains("VariableName"))
    {
        SetName(metaNode.Get<String>("VariableName"));
    }

    if (metaNode.Contains("VariableType"))
    {
        SetType(metaNode.Get<Variant::Type>("VariableType"));
    }

    Variant variant;
    if (metaNode.Contains("Variant"))
    {
        variant = metaNode.Get<Variant>("Variant");
    }
    variant.SetType(GetType());
    SetVariant(variant);
}

void AnimatorStateMachineVariable::ExportMeta(MetaNode *metaNode) const
{
    Serializable::ExportMeta(metaNode);

    metaNode->Set("VariableName", GetName());
    metaNode->Set("VariableType", GetType());
    metaNode->Set("Variant", GetVariant());
}
