#ifndef ANIMATORSTATEMACHINECONNECTION_H
#define ANIMATORSTATEMACHINECONNECTION_H

#include "Bang/Bang.h"
#include "Bang/DPtr.h"
#include "Bang/Serializable.h"
#include "Bang/AnimatorStateMachineConnectionTransitionCondition.h"

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
    bool AreTransitionConditionsFulfilled(
                        AnimatorStateMachine *animatorSM) const;

    void SetTransitionDuration(Time transitionDuration);
    void SetImmediateTransition(bool immediateTransition);
    ASMCTransitionCondition* CreateAndAddTransitionCondition();
    void RemoveTransitionCondition(ASMCTransitionCondition *transitionCond);
    void RemoveTransitionCondition(uint idx);

    AnimatorStateMachineNode* GetNodeTo() const;
    AnimatorStateMachineNode* GetNodeFrom() const;
    bool GetImmediateTransition() const;
    Time GetTransitionDuration() const;

    const Array<ASMCTransitionCondition*>& GetTransitionConditions() const;
    void CloneInto(AnimatorStateMachineConnection *cloneConnection) const;

    // Serializable
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;

private:
    DPtr<AnimatorStateMachineNode> p_nodeTo;
    DPtr<AnimatorStateMachineNode> p_nodeFrom;
    Time m_transitionDuration = Time::Seconds(0.5);
    bool m_immediateTransition = false;
    Array<ASMCTransitionCondition*> m_transitionConditions;

    DPtr<AnimatorStateMachine> p_stateMachine;
};

NAMESPACE_BANG_END

#endif // ANIMATORSTATEMACHINECONNECTION_H

