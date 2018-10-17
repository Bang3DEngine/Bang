#ifndef IEVENTSANIMATORSTATEMACHINENODE_H
#define IEVENTSANIMATORSTATEMACHINENODE_H

#include "Bang/IEvents.h"

namespace Bang
{
class AnimatorStateMachineNode;
class AnimatorStateMachineConnection;

class IEventsAnimatorStateMachineNode
{
    IEVENTS(IEventsAnimatorStateMachineNode);

public:
    virtual void OnConnectionAdded(AnimatorStateMachineNode *node,
                                   AnimatorStateMachineConnection *connection)
    {
        BANG_UNUSED_2(node, connection);
    }

    virtual void OnConnectionRemoved(AnimatorStateMachineNode *node,
                                     AnimatorStateMachineConnection *connection)
    {
        BANG_UNUSED_2(node, connection);
    }
};
}

#endif  // IEVENTSANIMATORSTATEMACHINENODE_H
