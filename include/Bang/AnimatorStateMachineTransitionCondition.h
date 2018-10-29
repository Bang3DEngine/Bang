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
class AnimatorStateMachineLayer;
class AnimatorStateMachineTransition;

class AnimatorStateMachineTransitionCondition : public Serializable
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

    AnimatorStateMachineTransitionCondition();
    virtual ~AnimatorStateMachineTransitionCondition() override;

    void SetVariableName(const String &variableName);
    void SetVariableType(AnimatorStateMachineVariable::Type type);
    void SetComparator(
        AnimatorStateMachineTransitionCondition::Comparator comparator);
    void SetCompareValueFloat(float compareValueFloat);

    bool IsFulfilled() const;
    const String &GetVariableName() const;
    AnimatorStateMachineVariable::Type GetVariableType() const;
    AnimatorStateMachineTransitionCondition::Comparator GetComparator() const;
    float GetCompareValueFloat() const;

    AnimatorStateMachine *GetStateMachine() const;
    AnimatorStateMachineLayer *GetLayer() const;
    AnimatorStateMachineTransition *GetTransition() const;

    // Serializable
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;

private:
    String m_varName = "";
    AnimatorStateMachineVariable::Type m_varType =
        AnimatorStateMachineVariable::Type::FLOAT;
    Comparator m_comparator = Comparator::GREATER;
    float m_compareValueFloat = 0.0f;
    AnimatorStateMachineTransition *p_transition = nullptr;

    void SetTransition(AnimatorStateMachineTransition *transition);

    friend class AnimatorStateMachineTransition;
};
}

#endif  // ANIMATORSTATEMACHINECONNECTIONTRANSITIONCONDITION_H