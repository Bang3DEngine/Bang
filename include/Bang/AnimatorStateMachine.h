#ifndef ANIMATORSTATEMACHINE_H
#define ANIMATORSTATEMACHINE_H

#include "Bang/Bang.h"
#include "Bang/Array.h"
#include "Bang/Resource.h"
#include "Bang/AnimatorStateMachineNode.h"

NAMESPACE_BANG_BEGIN

FORWARD class Animator;

class AnimatorStateMachine : public Resource
{
    RESOURCE(AnimatorStateMachine)

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

    const Array<AnimatorStateMachineNode>& GetNodes() const;

    // Resource
    virtual void Import(const Path &resourceFilepath) override;

    // Serializable
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;

private:
    AnimatorStateMachineNode m_entryNode;
    Array<AnimatorStateMachineNode> m_nodes;

    void CreateNodeInto(AnimatorStateMachineNode *node);
};

NAMESPACE_BANG_END

#endif // ANIMATORSTATEMACHINE_H

