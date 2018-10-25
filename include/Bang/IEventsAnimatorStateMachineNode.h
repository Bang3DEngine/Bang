#ifndef IEVENTSANIMATORSTATEMACHINENODE_H
#define IEVENTSANIMATORSTATEMACHINENODE_H

#include "Bang/IEvents.h"

namespace Bang
{
class AnimatorStateMachineNode;
class AnimatorStateMachineTransition;

class IEventsAnimatorStateMachineNode
{
    IEVENTS(IEventsAnimatorStateMachineNode);

public:
    virtual void OnTransitionAdded(AnimatorStateMachineNode *node,
                                   AnimatorStateMachineTransition *transition)
    {
        BANG_UNUSED_2(node, transition);
    }

    virtual void OnTransitionRemoved(AnimatorStateMachineNode *node,
                                     AnimatorStateMachineTransition *transition)
    {
        BANG_UNUSED_2(node, transition);
    }
};
}

#endif  // IEVENTSANIMATORSTATEMACHINENODE_H
