#ifndef IEVENTSANIMATORSTATEMACHINE_H
#define IEVENTSANIMATORSTATEMACHINE_H

#include "Bang/IEvents.h"

namespace Bang
{
class Animator;
class Animation;
class AnimatorStateMachine;
class AnimatorStateMachineLayer;
class AnimatorStateMachineNode;
class AnimatorStateMachineVariable;

class IEventsAnimatorStateMachine
{
    IEVENTS(IEventsAnimatorStateMachine);

public:
    virtual void OnLayerAdded(AnimatorStateMachine *stateMachine,
                              AnimatorStateMachineLayer *stateMachineLayer)
    {
        BANG_UNUSED_2(stateMachine, stateMachineLayer);
    }

    virtual void OnLayerRemoved(AnimatorStateMachine *stateMachine,
                                AnimatorStateMachineLayer *stateMachineLayer)
    {
        BANG_UNUSED_2(stateMachine, stateMachineLayer);
    }

    virtual void OnVariableNameChanged(AnimatorStateMachineVariable *variable,
                                       const String &prevVariableName,
                                       const String &nextVariableName)
    {
        BANG_UNUSED_3(variable, prevVariableName, nextVariableName);
    }
};
}

#endif  // IEVENTSANIMATORSTATEMACHINE_H
