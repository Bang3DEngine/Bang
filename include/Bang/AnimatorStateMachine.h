#ifndef ANIMATORSTATEMACHINE_H
#define ANIMATORSTATEMACHINE_H

#include "Bang/Array.h"
#include "Bang/Array.tcc"
#include "Bang/Asset.h"
#include "Bang/BangDefines.h"
#include "Bang/EventEmitter.h"
#include "Bang/EventListener.h"
#include "Bang/EventListener.tcc"
#include "Bang/IEventsAnimatorStateMachine.h"
#include "Bang/IEventsDestroy.h"
#include "Bang/MetaNode.h"
#include "Bang/String.h"

namespace Bang
{
class AnimatorStateMachine : public Asset,
                             public EventEmitter<IEventsAnimatorStateMachine>,
                             public EventListener<IEventsAnimatorStateMachine>
{
    ASSET(AnimatorStateMachine)

public:
    AnimatorStateMachine();
    virtual ~AnimatorStateMachine() override;

    AnimatorStateMachineLayer *CreateNewLayer();
    void MoveLayer(uint oldIndex, uint newIndex);
    void RemoveLayer(AnimatorStateMachineLayer *layer);

    AnimatorStateMachineVariable *CreateNewVariable();
    void SetVariableFloatDefaultValue(const String &varName, const float value);
    void SetVariableBoolDefaultValue(const String &varName, const bool value);
    void MoveVariable(uint oldIndex, uint newIndex);
    void RemoveVariable(AnimatorStateMachineVariable *var);
    void RemoveVariable(uint varIdx);

    AnimatorStateMachineVariable *GetVariableDefault(
        const String &varName) const;
    float GetVariableFloatDefaultValue(const String &varName) const;
    bool GetVariableBoolDefaultValue(const String &varName) const;
    Array<String> GetVariablesNames() const;
    const Array<AnimatorStateMachineVariable *> &GetVariableDefaults() const;
    const Array<AnimatorStateMachineLayer *> &GetLayers() const;

    void Clear();

    // Asset
    virtual void Import(const Path &assetFilepath) override;

    // Serializable
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;

private:
    Array<AnimatorStateMachineLayer *> m_layers;
    Array<AnimatorStateMachineVariable *> m_variables;

    AnimatorStateMachineVariable *CreateOrGetVariable(const String &varName);

    // IEventsAnimatorStateMachine
    virtual void OnVariableNameChanged(AnimatorStateMachineVariable *variable,
                                       const String &prevVariableName,
                                       const String &nextVariableName) override;
};
}

#endif  // ANIMATORSTATEMACHINE_H
