#include "Bang/AnimatorStateMachineVariable.h"

#include "Bang/MetaNode.h"
#include "Bang/AnimatorStateMachine.h"

USING_NAMESPACE_BANG

AnimatorStateMachineVariable::AnimatorStateMachineVariable()
{
}

AnimatorStateMachineVariable::~AnimatorStateMachineVariable()
{
}

void AnimatorStateMachineVariable::SetType(Type type)
{
    m_type = type;
}

void AnimatorStateMachineVariable::SetValueFloat(float value)
{
    m_valueFloat = value;
}

void AnimatorStateMachineVariable::SetValueBool(bool value)
{
    m_valueBool = value;
}

const String &AnimatorStateMachineVariable::GetName() const
{
    return m_name;
}

AnimatorStateMachineVariable::Type AnimatorStateMachineVariable::GetType() const
{
    return m_type;
}

bool AnimatorStateMachineVariable::GetValueBool() const
{
    return m_valueBool;
}

float AnimatorStateMachineVariable::GetValueFloat() const
{
    return m_valueFloat;
}

void AnimatorStateMachineVariable::SetName(const String &varName)
{
    String prevName = GetName();

    m_name = varName;

    p_animatorSM->EventEmitter<IEventsAnimatorStateMachine>::PropagateToListeners(
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
        SetName( metaNode.Get<String>("VariableName") );
    }

    if (metaNode.Contains("VariableType"))
    {
        SetType( metaNode.Get<AnimatorStateMachineVariable::Type>("VariableType") );
    }

    if (metaNode.Contains("ValueFloat"))
    {
        SetValueFloat( metaNode.Get<float>("ValueFloat") );
    }

    if (metaNode.Contains("ValueBool"))
    {
        SetValueBool( metaNode.Get<bool>("ValueBool") );
    }
}

void AnimatorStateMachineVariable::ExportMeta(MetaNode *metaNode) const
{
    Serializable::ExportMeta(metaNode);

    metaNode->Set("VariableName", GetName());
    metaNode->Set("VariableType", GetType());
    metaNode->Set("ValueFloat",   GetValueFloat());
    metaNode->Set("ValueBool",    GetValueBool());
}

