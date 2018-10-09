#ifndef IEVENTSANIMATORSTATEMACHINE_H
#define IEVENTSANIMATORSTATEMACHINE_H

#include "Bang/Time.h"
#include "Bang/IEvents.h"

NAMESPACE_BANG_BEGIN

FORWARD class Animator;
FORWARD class Animation;
FORWARD class AnimatorStateMachine;
FORWARD class AnimatorStateMachineNode;

class IEventsAnimatorStateMachine
{
    IEVENTS(IEventsAnimatorStateMachine)

public:
    virtual void OnNodeCreated(AnimatorStateMachine *stateMachine,
                               uint newNodeIdx,
                               AnimatorStateMachineNode *newNode)
    {
        BANG_UNUSED_3(stateMachine, newNodeIdx, newNode);
    }

    virtual void OnNodeRemoved(AnimatorStateMachine *stateMachine,
                               uint removedNodeIdx,
                               AnimatorStateMachineNode *removedNode)
    {
        BANG_UNUSED_3(stateMachine, removedNodeIdx, removedNode);
    }

    virtual void OnAnimationTick(Animator *animator,
                                 Animation *animation,
                                 uint animationIndex,
                                 Time animationTimeWrapped,
                                 Time animationTime)
    {
        BANG_UNUSED_5(animator,
                      animationIndex,
                      animation,
                      animationTimeWrapped,
                      animationTime);
    }
};

NAMESPACE_BANG_END

#endif // IEVENTSANIMATORSTATEMACHINE_H
