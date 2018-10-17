#ifndef IEVENTSANIMATORSTATEMACHINE_H
#define IEVENTSANIMATORSTATEMACHINE_H

#include "Bang/IEvents.h"
#include "Bang/Time.h"

namespace Bang
{
class Animator;
class Animation;
class AnimatorStateMachine;
class AnimatorStateMachineNode;
class AnimatorStateMachineVariable;

class IEventsAnimatorStateMachine
{
    IEVENTS(IEventsAnimatorStateMachine);

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

    virtual void OnVariableNameChanged(AnimatorStateMachineVariable *variable,
                                       const String &prevVariableName,
                                       const String &nextVariableName)
    {
        BANG_UNUSED_3(variable, prevVariableName, nextVariableName);
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
}

#endif  // IEVENTSANIMATORSTATEMACHINE_H
