#ifndef ANIMATORSTATEMACHINETRANSITION_H
#define ANIMATORSTATEMACHINETRANSITION_H

#include <vector>

#include "Bang/AnimatorStateMachineNode.h"
#include "Bang/Array.h"
#include "Bang/Array.tcc"
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

namespace Bang
{
class AnimatorStateMachine;
class AnimatorStateMachineLayer;
class AnimatorStateMachineTransitionCondition;
class IEventsDestroy;

class AnimatorStateMachineTransition
    : public Serializable,
      public EventEmitterIEventsDestroyWithCheck
{
    SERIALIZABLE(AnimatorStateMachineConnection)

public:
    AnimatorStateMachineTransition();
    virtual ~AnimatorStateMachineTransition() override;

    void SetNodeTo(AnimatorStateMachineNode *node);
    void SetNodeFrom(AnimatorStateMachineNode *node);
    bool AreTransitionConditionsFulfilled(Animator *animator) const;

    void SetTransitionDuration(Time transitionDuration);
    void SetWaitForAnimationToFinish(bool waitForAnimationToFinish);
    AnimatorStateMachineTransitionCondition *CreateAndAddTransitionCondition();
    void RemoveTransitionCondition(
        AnimatorStateMachineTransitionCondition *transitionCond);
    void RemoveTransitionCondition(uint idx);

    AnimatorStateMachineNode *GetNodeTo() const;
    AnimatorStateMachineNode *GetNodeFrom() const;
    bool GetWaitForAnimationToFinish() const;
    Time GetTransitionDuration() const;

    const Array<AnimatorStateMachineTransitionCondition *>
        &GetTransitionConditions() const;

    AnimatorStateMachine *GetStateMachine() const;
    AnimatorStateMachineLayer *GetLayer() const;

    // ICloneable
    void CloneInto(ICloneable *clone, bool cloneGUID) const override;

    // Serializable
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;

private:
    DPtr<AnimatorStateMachineNode> p_nodeTo;
    DPtr<AnimatorStateMachineNode> p_nodeFrom;
    Time m_transitionDuration = Time::Seconds(0.5);
    bool m_waitForAnimationToFinish = false;
    Array<AnimatorStateMachineTransitionCondition *> m_transitionConditions;
};
}  // namespace Bang

#endif  // ANIMATORSTATEMACHINETRANSITION_H
