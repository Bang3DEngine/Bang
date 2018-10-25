#ifndef IEVENTSANIMATORSTATEMACHINELAYER_H
#define IEVENTSANIMATORSTATEMACHINELAYER_H

#include "Bang/IEvents.h"

namespace Bang
{
class Animator;
class Animation;
class AnimatorStateMachine;
class AnimatorStateMachineLayer;
class AnimatorStateMachineNode;
class AnimatorStateMachineVariable;

class IEventsAnimatorStateMachineLayer
{
    IEVENTS(IEventsAnimatorStateMachineLayer);

public:
    virtual void OnNodeCreated(uint newNodeIdx,
                               AnimatorStateMachineNode *newNode)
    {
        BANG_UNUSED_2(newNodeIdx, newNode);
    }

    virtual void OnNodeRemoved(uint removedNodeIdx,
                               AnimatorStateMachineNode *removedNode)
    {
        BANG_UNUSED_2(removedNodeIdx, removedNode);
    }
};
}

#endif  // IEVENTSANIMATORSTATEMACHINELAYER_H
