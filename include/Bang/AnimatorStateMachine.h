#ifndef ANIMATORSTATEMACHINE_H
#define ANIMATORSTATEMACHINE_H

#include "Bang/Bang.h"
#include "Bang/Array.h"
#include "Bang/AnimatorStateMachineNode.h"

NAMESPACE_BANG_BEGIN

FORWARD class Animator;

class AnimatorStateMachine
{
public:
	AnimatorStateMachine();
	virtual ~AnimatorStateMachine();

    AnimatorStateMachineNode* CreateNode();
    AnimatorStateMachineNode* GetEntryNode();
    const AnimatorStateMachineNode* GetEntryNode() const;
    const AnimatorStateMachineNode* GetNode(uint nodeIdx) const;
    AnimatorStateMachineNode* GetNode(uint nodeIdx);
    void RemoveNode(AnimatorStateMachineNode *node);
    void RemoveNode(uint idx);

    void SetAnimator(Animator *animator);

    Animator* GetAnimator() const;
    const Array<AnimatorStateMachineNode>& GetNodes() const;

private:
    Animator *p_animator = nullptr;
    AnimatorStateMachineNode m_entryNode;
    Array<AnimatorStateMachineNode> m_nodes;

    void CreateNodeInto(AnimatorStateMachineNode *node);
};

NAMESPACE_BANG_END

#endif // ANIMATORSTATEMACHINE_H

