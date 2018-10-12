#ifndef ANIMATORSTATEMACHINE_H
#define ANIMATORSTATEMACHINE_H

#include "Bang/Bang.h"
#include "Bang/Array.h"
#include "Bang/Resource.h"
#include "Bang/EventEmitter.h"
#include "Bang/AnimatorStateMachineNode.h"
#include "Bang/IEventsAnimatorStateMachine.h"
#include "Bang/AnimatorStateMachineVariable.h"

NAMESPACE_BANG_BEGIN

FORWARD class Animator;

class AnimatorStateMachine : public Resource,
                             public EventEmitter<IEventsDestroy>,
                             public EventEmitter<IEventsAnimatorStateMachine>,
                             public EventListener<IEventsAnimatorStateMachine>
{
    RESOURCE(AnimatorStateMachine)

public:
	AnimatorStateMachine();
	virtual ~AnimatorStateMachine();

    AnimatorStateMachineNode* CreateAndAddNode();
    AnimatorStateMachineNode* GetNode(uint nodeIdx);
    const AnimatorStateMachineNode* GetNode(uint nodeIdx) const;
    void RemoveNode(AnimatorStateMachineNode *nodeToRemove);

    AnimatorStateMachineVariable* CreateNewVariable();
    void SetVariableFloat(const String &varName, const float value);
    void SetVariableBool(const String &varName, const bool value);
    void RemoveVariable(AnimatorStateMachineVariable *var);
    void RemoveVariable(uint varIdx);
    void Clear();

    AnimatorStateMachineVariable* GetVariable(const String &varName) const;
    float GetVariableFloat(const String &varName) const;
    bool  GetVariableBool(const String &varName) const;

    const Array<AnimatorStateMachineNode*>& GetNodes() const;
    const Array<AnimatorStateMachineVariable*>& GetVariables() const;
    Array<String> GetVariablesNames() const;

    // Resource
    virtual void Import(const Path &resourceFilepath) override;

    // Serializable
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;

private:
    Array<AnimatorStateMachineNode*> m_nodes;
    Array<AnimatorStateMachineVariable*> m_variables;

    AnimatorStateMachineVariable* CreateOrGetVariable(const String &varName);

    // IEventsAnimatorStateMachine
    virtual void OnVariableNameChanged(AnimatorStateMachineVariable *variable,
                                       const String &prevVariableName,
                                       const String &nextVariableName) override;
};

NAMESPACE_BANG_END

#endif // ANIMATORSTATEMACHINE_H

