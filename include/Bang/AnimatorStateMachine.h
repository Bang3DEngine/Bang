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
                             public EventEmitter<IEventsAnimatorStateMachine>
{
    RESOURCE(AnimatorStateMachine)

public:
	AnimatorStateMachine();
	virtual ~AnimatorStateMachine();

    uint GetCurrentNodeIndex() const;
    AnimatorStateMachineNode* CreateAndAddNode();
    AnimatorStateMachineNode* GetCurrentNode();
    const AnimatorStateMachineNode* GetCurrentNode() const;
    const AnimatorStateMachineNode* GetNode(uint nodeIdx) const;
    AnimatorStateMachineNode* GetNode(uint nodeIdx);
    void RemoveNode(uint idx);

    const Array<AnimatorStateMachineNode>& GetNodes() const;

    // Resource
    virtual void Import(const Path &resourceFilepath) override;

    // Serializable
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;

private:
    uint m_currentNodeIndex = -1u;
    Array<AnimatorStateMachineNode> m_nodes;

    void CreateNodeInto(AnimatorStateMachineNode *node);
};

NAMESPACE_BANG_END

#endif // ANIMATORSTATEMACHINE_H

