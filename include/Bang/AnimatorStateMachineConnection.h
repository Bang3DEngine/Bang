#ifndef ANIMATORSTATEMACHINECONNECTION_H
#define ANIMATORSTATEMACHINECONNECTION_H

#include "Bang/Bang.h"
#include "Bang/Serializable.h"

NAMESPACE_BANG_BEGIN

FORWARD class AnimatorStateMachine;
FORWARD class AnimatorStateMachineNode;

class AnimatorStateMachineConnection : public Serializable
{
    SERIALIZABLE(AnimatorStateMachineConnection)

public:
    AnimatorStateMachineConnection();
    virtual ~AnimatorStateMachineConnection();

    void SetNodeToIndex(uint nodeToIdx);
    void SetNodeFromIndex(uint nodeFromIdx);
    void SetNodeTo(AnimatorStateMachineNode* node);
    void SetNodeFrom(AnimatorStateMachineNode* node);

    uint GetNodeToIndex() const;
    uint GetNodeFromIndex() const;
    AnimatorStateMachineNode* GetNodeTo() const;
    AnimatorStateMachineNode* GetNodeFrom() const;
    AnimatorStateMachine *GetStateMachine() const;
    uint GetIndexInsideNodeConnections(uint nodeIdx) const;

    void CloneInto(AnimatorStateMachineConnection *cloneConnection) const;

    // Serializable
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;

private:
    uint m_nodeToIndex   = -1u;
    uint m_nodeFromIndex = -1u;
    AnimatorStateMachine *p_stateMachine = nullptr;

    AnimatorStateMachineNode *GetSMNode(uint idx) const;
    uint GetSMNodeIdx(const AnimatorStateMachineNode *node) const;
    void SetStateMachine(AnimatorStateMachine *stateMachine);

    friend class AnimatorStateMachine;
    friend class AnimatorStateMachineNode;
};

NAMESPACE_BANG_END

#endif // ANIMATORSTATEMACHINECONNECTION_H

