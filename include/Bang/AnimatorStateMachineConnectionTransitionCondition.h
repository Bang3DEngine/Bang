#ifndef ANIMATORSTATEMACHINECONNECTIONTRANSITIONCONDITION_H
#define ANIMATORSTATEMACHINECONNECTIONTRANSITIONCONDITION_H

#include "Bang/AnimatorStateMachineVariable.h"
#include "Bang/BangDefines.h"
#include "Bang/MetaNode.h"
#include "Bang/Serializable.h"
#include "Bang/String.h"

namespace Bang
{
class AnimatorStateMachine;
class AnimatorStateMachineConnectionTransitionCondition;

using ASMCTransitionCondition =
    AnimatorStateMachineConnectionTransitionCondition;

class AnimatorStateMachineConnectionTransitionCondition : public Serializable
{
    SERIALIZABLE(AnimatorStateMachineConnectionTransitionCondition)

public:
    enum class Comparator
    {
        GREATER,
        LESS,
        IS_TRUE,
        IS_FALSE
    };

    AnimatorStateMachineConnectionTransitionCondition();
    virtual ~AnimatorStateMachineConnectionTransitionCondition() override;

    void SetVariableName(const String &variableName);
    void SetVariableType(AnimatorStateMachineVariable::Type type);
    void SetComparator(ASMCTransitionCondition::Comparator comparator);
    void SetCompareValueFloat(float compareValueFloat);

    const String &GetVariableName() const;
    AnimatorStateMachineVariable::Type GetVariableType() const;
    ASMCTransitionCondition::Comparator GetComparator() const;
    float GetCompareValueFloat() const;

    bool IsFulfilled(AnimatorStateMachine *stateMachine) const;

    // Serializable
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;

private:
    String m_varName = "";
    AnimatorStateMachineVariable::Type m_varType =
        AnimatorStateMachineVariable::Type::FLOAT;
    Comparator m_comparator = Comparator::GREATER;
    float m_compareValueFloat = 0.0f;
};
}

#endif  // ANIMATORSTATEMACHINECONNECTIONTRANSITIONCONDITION_H
