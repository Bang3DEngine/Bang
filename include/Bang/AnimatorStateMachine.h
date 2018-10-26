#ifndef ANIMATORSTATEMACHINE_H
#define ANIMATORSTATEMACHINE_H

#include "Bang/Array.h"
#include "Bang/Array.tcc"
#include "Bang/BangDefines.h"
#include "Bang/EventEmitter.h"
#include "Bang/EventListener.h"
#include "Bang/EventListener.tcc"
#include "Bang/IEventsAnimatorStateMachine.h"
#include "Bang/IEventsDestroy.h"
#include "Bang/MetaNode.h"
#include "Bang/Resource.h"
#include "Bang/String.h"

namespace Bang
{
class AnimatorStateMachine : public Resource,
                             public EventEmitterIEventsDestroyWithCheck,
                             public EventEmitter<IEventsAnimatorStateMachine>,
                             public EventListener<IEventsAnimatorStateMachine>
{
    RESOURCE(AnimatorStateMachine)

public:
    AnimatorStateMachine();
    virtual ~AnimatorStateMachine();

    AnimatorStateMachineLayer *CreateNewLayer();
    void MoveLayer(uint oldIndex, uint newIndex);
    void RemoveLayer(AnimatorStateMachineLayer *layer);

    AnimatorStateMachineVariable *CreateNewVariable();
    void SetVariableFloat(const String &varName, const float value);
    void SetVariableBool(const String &varName, const bool value);
    void MoveVariable(uint oldIndex, uint newIndex);
    void RemoveVariable(AnimatorStateMachineVariable *var);
    void RemoveVariable(uint varIdx);

    AnimatorStateMachineVariable *GetVariable(const String &varName) const;
    float GetVariableFloat(const String &varName) const;
    bool GetVariableBool(const String &varName) const;
    Array<String> GetVariablesNames() const;
    const Array<AnimatorStateMachineVariable *> &GetVariables() const;
    const Array<AnimatorStateMachineLayer *> &GetLayers() const;

    void Clear();

    // Resource
    virtual void Import(const Path &resourceFilepath) override;

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
