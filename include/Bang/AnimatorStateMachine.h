#ifndef ANIMATORSTATEMACHINE_H
#define ANIMATORSTATEMACHINE_H

#include <vector>

#include "Bang/Array.h"
#include "Bang/Array.tcc"
#include "Bang/BangDefines.h"
#include "Bang/EventEmitter.h"
#include "Bang/EventListener.h"
#include "Bang/EventListener.tcc"
#include "Bang/IEventsAnimatorStateMachine.h"
#include "Bang/MetaNode.h"
#include "Bang/Resource.h"
#include "Bang/String.h"

NAMESPACE_BANG_BEGIN

FORWARD class AnimatorStateMachineNode;
FORWARD class AnimatorStateMachineVariable;
FORWARD class IEventsAnimatorStateMachine;
FORWARD class IEventsDestroy;
FORWARD class Path;

class AnimatorStateMachine : public Resource,
                             public EventEmitter<IEventsDestroy>,
                             public EventEmitter<IEventsAnimatorStateMachine>,
                             public EventListener<IEventsAnimatorStateMachine>
{
    RESOURCE(AnimatorStateMachine)

public:
	AnimatorStateMachine();
	virtual ~AnimatorStateMachine() override;

    AnimatorStateMachineNode* CreateAndAddNode();
    AnimatorStateMachineNode* GetNode(uint nodeIdx);
    const AnimatorStateMachineNode* GetNode(uint nodeIdx) const;
    void RemoveNode(AnimatorStateMachineNode *nodeToRemove);
    void SetEntryNode(AnimatorStateMachineNode *entryNode);
    void SetEntryNodeIdx(uint entryNodeIdx);

    AnimatorStateMachineVariable* CreateNewVariable();
    void SetVariableFloat(const String &varName, const float value);
    void SetVariableBool(const String &varName, const bool value);
    void RemoveVariable(AnimatorStateMachineVariable *var);
    void RemoveVariable(uint varIdx);
    void Clear();

    AnimatorStateMachineVariable* GetVariable(const String &varName) const;
    float GetVariableFloat(const String &varName) const;
    bool  GetVariableBool(const String &varName) const;
    AnimatorStateMachineNode* GetEntryNodeOrFirstFound() const;
    AnimatorStateMachineNode* GetEntryNode() const;
    uint GetEntryNodeIdx() const;

    const Array<AnimatorStateMachineNode*>& GetNodes() const;
    const Array<AnimatorStateMachineVariable*>& GetVariables() const;
    Array<String> GetVariablesNames() const;

    // Resource
    virtual void Import(const Path &resourceFilepath) override;

    // Serializable
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;

private:
    uint m_entryNodeIdx = -1u;
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

