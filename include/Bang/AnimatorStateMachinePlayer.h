#ifndef ANIMATORSTATEMACHINEPLAYER_H
#define ANIMATORSTATEMACHINEPLAYER_H

#include <vector>

#include "Bang/AnimatorStateMachineLayer.h"
#include "Bang/AnimatorStateMachineNode.h"
#include "Bang/AnimatorStateMachineTransition.h"
#include "Bang/Array.tcc"
#include "Bang/AssetHandle.h"
#include "Bang/BangDefines.h"
#include "Bang/DPtr.h"
#include "Bang/EventEmitter.tcc"
#include "Bang/EventListener.h"
#include "Bang/IEventsAnimatorStateMachineLayer.h"
#include "Bang/IEventsAnimatorStateMachineNode.h"
#include "Bang/Time.h"

namespace Bang
{
class Animation;
class AnimatorStateMachineTransition;
class IEventsAnimatorStateMachine;
class IEventsAnimatorStateMachineNode;

class AnimatorStateMachinePlayer
    : public EventListener<IEventsAnimatorStateMachineLayer>,
      public EventListener<IEventsAnimatorStateMachineNode>
{
public:
    AnimatorStateMachinePlayer();
    virtual ~AnimatorStateMachinePlayer() override;

    void SetStateMachineLayer(AnimatorStateMachineLayer *stateMachineLayer);

    void Step(Animator *animator, Time deltaTime);
    void SetCurrentNode(AnimatorStateMachineNode *node);
    void SetCurrentNode(AnimatorStateMachineNode *node, Time nodeTime);

    void StartTransition(AnimatorStateMachineTransition *connection,
                         Time prevNodeTime,
                         Time startTransitionTime = Time(0));
    void FinishCurrentTransition(Animator *animator);

    AnimatorStateMachineNode *GetCurrentNode() const;
    Animation *GetCurrentAnimation() const;
    Time GetCurrentNodeTime() const;

    AnimatorStateMachineNode *GetNextNode() const;
    Animation *GetNextAnimation() const;
    Time GetNextNodeTime() const;

    AnimatorStateMachineTransition *GetCurrentTransition() const;
    Time GetCurrentTransitionTime() const;
    Time GetCurrentTransitionDuration() const;

    AnimatorStateMachine *GetStateMachine() const;
    AnimatorStateMachineLayer *GetStateMachineLayer() const;

private:
    DPtr<AnimatorStateMachineLayer> p_stateMachineLayer = nullptr;

    Time m_currentNodeTime;
    DPtr<AnimatorStateMachineNode> p_currentNode = nullptr;

    Time m_currentTransitionTime;
    DPtr<AnimatorStateMachineTransition> p_currentTransition = nullptr;

    // IEventsAnimatorStateMachineLayer
    virtual void OnNodeCreated(uint newNodeIdx,
                               AnimatorStateMachineNode *newNode) override;
    virtual void OnNodeRemoved(uint removedNodeIdx,
                               AnimatorStateMachineNode *removedNode) override;

    // IEventsAnimatorStateMachineNode
    virtual void OnTransitionAdded(
        AnimatorStateMachineNode *node,
        AnimatorStateMachineTransition *transition) override;
    virtual void OnTransitionRemoved(
        AnimatorStateMachineNode *node,
        AnimatorStateMachineTransition *transition) override;
};
}

#endif  // ANIMATORSTATEMACHINEPLAYER_H
