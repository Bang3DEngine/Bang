#ifndef ANIMATORSTATEMACHINECONNECTION_H
#define ANIMATORSTATEMACHINECONNECTION_H

#include <vector>

#include "Bang/AnimatorStateMachineConnectionTransitionCondition.h"
#include "Bang/Array.h"
#include "Bang/Array.tcc"
#include "Bang/Bang.h"
#include "Bang/BangDefines.h"
#include "Bang/DPtr.h"
#include "Bang/EventEmitter.h"
#include "Bang/EventEmitter.tcc"
#include "Bang/EventListener.tcc"
#include "Bang/IEvents.h"
#include "Bang/MetaNode.h"
#include "Bang/Serializable.h"
#include "Bang/String.h"
#include "Bang/Time.h"

NAMESPACE_BANG_BEGIN

FORWARD class AnimatorStateMachine;
FORWARD class AnimatorStateMachineNode;
FORWARD class IEventsDestroy;

class AnimatorStateMachineConnection : public Serializable,
                                       public EventEmitter<IEventsDestroy>
{
    SERIALIZABLE(AnimatorStateMachineConnection)

public:
    AnimatorStateMachineConnection(AnimatorStateMachine *stateMachine);
    virtual ~AnimatorStateMachineConnection() override;

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

