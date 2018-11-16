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
    AnimatorStateMachineVariable();
    virtual ~AnimatorStateMachineVariable() override;

    void SetName(const String &varName);
    void SetType(Variant::Type type);
    void SetValueFloat(float value);
    void SetValueBool(bool value);
    void SetVariant(const Variant &variant);

    const String &GetName() const;
    Variant::Type GetType() const;
    bool GetValueBool() const;
    float GetValueFloat() const;
    const Variant &GetVariant() const;
    AnimatorStateMachine *GetStateMachine() const;

private:
    AnimatorStateMachine *p_stateMachine = nullptr;
    String m_name = "EmptyName";
    Variant m_variant;

    void SetStateMachine(AnimatorStateMachine *stateMachine);

    // Serializable
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;

    friend class AnimatorStateMachine;
};
}

#endif  // ANIMATORSTATEMACHINEVARIABLE_H
