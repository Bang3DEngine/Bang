#ifndef ANIMATORSTATEMACHINE_H
#define ANIMATORSTATEMACHINE_H

#include "Bang/Bang.h"
#include "Bang/Array.h"
#include "Bang/Resource.h"
#include "Bang/EventEmitter.h"
#include "Bang/AnimatorStateMachineNode.h"
#include "Bang/IEventsAnimatorStateMachine.h"

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
    AnimatorStateMachineNode* GetCurrentNode();
    const AnimatorStateMachineNode* GetCurrentNode() const;
    AnimatorStateMachineNode* GetNode(uint nodeIdx);
    const AnimatorStateMachineNode* GetNode(uint nodeIdx) const;
    void RemoveNode(AnimatorStateMachineNode *nodeToRemove);

    void Clear();
    const Array<AnimatorStateMachineNode*>& GetNodes() const;

    // Resource
    virtual void Import(const Path &resourceFilepath) override;

    // Serializable
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;

private:
    AnimatorStateMachineNode *p_currentNode = nullptr;
    Array<AnimatorStateMachineNode*> m_nodes;
};

NAMESPACE_BANG_END

#endif // ANIMATORSTATEMACHINE_H

