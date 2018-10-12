#ifndef ANIMATORSTATEMACHINEPLAYER_H
#define ANIMATORSTATEMACHINEPLAYER_H

#include "Bang/Bang.h"
#include "Bang/Time.h"
#include "Bang/EventEmitter.h"
#include "Bang/EventListener.h"
#include "Bang/ResourceHandle.h"
#include "Bang/IEventsAnimatorStateMachine.h"
#include "Bang/IEventsAnimatorStateMachineNode.h"

NAMESPACE_BANG_BEGIN

FORWARD class AnimatorStateMachine;
FORWARD class AnimatorStateMachineNode;

class AnimatorStateMachinePlayer :
                public EventListener<IEventsAnimatorStateMachine>,
                public EventListener<IEventsAnimatorStateMachineNode>
{
public:
	AnimatorStateMachinePlayer();
	virtual ~AnimatorStateMachinePlayer();

    void SetStateMachine(AnimatorStateMachine *stateMachine);

    void Step(Time deltaTime);
    void SetCurrentNode(AnimatorStateMachineNode *node);
    void SetCurrentNode(AnimatorStateMachineNode *node, Time nodeTime);

    void StartTransition(AnimatorStateMachineConnection *connection,
                         Time prevNodeTime,
                         Time startTransitionTime = Time(0));
    void FinishCurrentTransition();

    AnimatorStateMachineNode* GetCurrentNode() const;
    Animation* GetCurrentAnimation() const;
    Time GetCurrentNodeTime() const;

    AnimatorStateMachineNode* GetNextNode() const;
    Animation* GetNextAnimation() const;
    Time GetNextNodeTime() const;

    AnimatorStateMachineConnection *GetCurrentTransition() const;
    Time GetCurrentTransitionTime() const;
    Time GetCurrentTransitionDuration() const;

    AnimatorStateMachine *GetStateMachine() const;

private:
    RH<AnimatorStateMachine> p_stateMachine;

    Time m_currentNodeTime;
    AnimatorStateMachineNode *p_currentNode = nullptr;

    Time m_currentTransitionTime;
    AnimatorStateMachineConnection *p_currentTransition = nullptr;

    // IEventsAnimatorStateMachine
    virtual void OnNodeCreated(AnimatorStateMachine *stateMachine,
                               uint newNodeIdx,
                               AnimatorStateMachineNode *newNode) override;
    virtual void OnNodeRemoved(AnimatorStateMachine *stateMachine,
                               uint removedNodeIdx,
                               AnimatorStateMachineNode *removedNode) override;


    // IEventsAnimatorStateMachineNode
    virtual void OnConnectionAdded(AnimatorStateMachineNode *node,
                                   AnimatorStateMachineConnection *connection)
                 override;

    virtual void OnConnectionRemoved(AnimatorStateMachineNode *node,
                                     AnimatorStateMachineConnection *connection)
                 override;
};

NAMESPACE_BANG_END

#endif // ANIMATORSTATEMACHINEPLAYER_H

