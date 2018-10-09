#ifndef ANIMATORSTATEMACHINECONNECTION_H
#define ANIMATORSTATEMACHINECONNECTION_H

#include "Bang/Bang.h"
#include "Bang/DPtr.h"
#include "Bang/Serializable.h"

NAMESPACE_BANG_BEGIN

FORWARD class AnimatorStateMachine;
FORWARD class AnimatorStateMachineNode;

class AnimatorStateMachineConnection : public Serializable,
                                       public EventEmitter<IEventsDestroy>
{
    SERIALIZABLE(AnimatorStateMachineConnection)

public:
    AnimatorStateMachineConnection(AnimatorStateMachine *stateMachine);
    virtual ~AnimatorStateMachineConnection();

    void SetNodeTo(AnimatorStateMachineNode* node);
    void SetNodeFrom(AnimatorStateMachineNode* node);

    AnimatorStateMachineNode* GetNodeTo() const;
    AnimatorStateMachineNode* GetNodeFrom() const;

    void CloneInto(AnimatorStateMachineConnection *cloneConnection) const;

    // Serializable
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;

private:
    DPtr<AnimatorStateMachineNode> p_nodeTo;
    DPtr<AnimatorStateMachineNode> p_nodeFrom;
    DPtr<AnimatorStateMachine> p_stateMachine;
};

NAMESPACE_BANG_END

#endif // ANIMATORSTATEMACHINECONNECTION_H

