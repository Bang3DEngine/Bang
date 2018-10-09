#ifndef ANIMATORSTATEMACHINEVARIABLE_H
#define ANIMATORSTATEMACHINEVARIABLE_H

#include "Bang/Bang.h"
#include "Bang/Serializable.h"

NAMESPACE_BANG_BEGIN

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
    virtual ~AnimatorStateMachineVariable();

    void SetType(AnimatorStateMachineVariable::Type type);
    void SetValueFloat(float value);
    void SetValueBool(bool value);

    Type GetType() const;
    bool GetValueBool() const;
    float GetValueFloat() const;

private:
    Type m_type = Type::FLOAT;
    float m_valueFloat = 0.0f;
    bool m_valueBool = false;

    // Serializable
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;
};

NAMESPACE_BANG_END

#endif // ANIMATORSTATEMACHINEVARIABLE_H

