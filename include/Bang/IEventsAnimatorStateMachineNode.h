#ifndef IEVENTSANIMATORSTATEMACHINENODE_H
#define IEVENTSANIMATORSTATEMACHINENODE_H

#include "Bang/IEvents.h"

NAMESPACE_BANG_BEGIN

FORWARD class AnimatorStateMachineNode;
FORWARD class AnimatorStateMachineConnection;

class IEventsAnimatorStateMachineNode
{
    IEVENTS(IEventsAnimatorStateMachineNode)

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

NAMESPACE_BANG_END

#endif // IEVENTSANIMATORSTATEMACHINENODE_H
