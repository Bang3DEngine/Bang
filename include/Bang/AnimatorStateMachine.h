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
                             public EventEmitter<IEventsAnimatorStateMachine>
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
    bool SetVariableName(AnimatorStateMachineVariable *var,
                         const String &newVarName);
    void RemoveVariable(AnimatorStateMachineVariable *var);
    void Clear();

    AnimatorStateMachineVariable* GetVariable(const String &varName) const;
    String GetVariableName(AnimatorStateMachineVariable* var);
    float GetVariableFloat(const String &varName) const;
    bool  GetVariableBool(const String &varName) const;

    const Array<AnimatorStateMachineNode*>& GetNodes() const;
    Array<AnimatorStateMachineVariable*> GetVariables() const;
    Array<String> GetVariablesNames() const;
    const Map<String, AnimatorStateMachineVariable*>& GetNameToVariables() const;

    // Resource
    virtual void Import(const Path &resourceFilepath) override;

    // Serializable
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;

private:
    Array<AnimatorStateMachineNode*> m_nodes;
    Map<String, AnimatorStateMachineVariable*> m_nameToVariable;
    Map<AnimatorStateMachineVariable*, String> m_variableToName;

    AnimatorStateMachineVariable* CreateOrGetVariable(const String &varName);
};

NAMESPACE_BANG_END

#endif // ANIMATORSTATEMACHINE_H

