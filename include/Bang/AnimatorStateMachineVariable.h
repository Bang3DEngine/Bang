#ifndef ANIMATORSTATEMACHINEVARIABLE_H
#define ANIMATORSTATEMACHINEVARIABLE_H

#include "Bang/BangDefines.h"
#include "Bang/MetaNode.h"
#include "Bang/Serializable.h"
#include "Bang/String.h"

namespace Bang
{
class AnimatorStateMachine;

class AnimatorStateMachineVariable : public Serializable
{
    SERIALIZABLE(AnimatorStateMachineVariable)

public:
    enum class Type
    {
        FLOAT,
        BOOL
    };

    AnimatorStateMachineVariable();
    virtual ~AnimatorStateMachineVariable() override;

    void SetName(const String &varName);
    void SetType(AnimatorStateMachineVariable::Type type);
    void SetValueFloat(float value);
    void SetValueBool(bool value);

    const String &GetName() const;
    Type GetType() const;
    bool GetValueBool() const;
    float GetValueFloat() const;

private:
    AnimatorStateMachine *p_animatorSM = nullptr;
    String m_name = "EmptyName";
    Type m_type = Type::FLOAT;
    float m_valueFloat = 0.0f;
    bool m_valueBool = false;

    // Serializable
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;

    friend class AnimatorStateMachine;
};
}

#endif  // ANIMATORSTATEMACHINEVARIABLE_H
