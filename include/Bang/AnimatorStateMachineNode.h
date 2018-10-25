#ifndef ANIMATORSTATEMACHINENODE_H
#define ANIMATORSTATEMACHINENODE_H

#include <vector>

#include "Bang/Array.h"
#include "Bang/Array.tcc"
#include "Bang/BangDefines.h"
#include "Bang/DPtr.h"
#include "Bang/EventEmitter.h"
#include "Bang/EventEmitter.tcc"
#include "Bang/EventListener.tcc"
#include "Bang/IEvents.h"
#include "Bang/MetaNode.h"
#include "Bang/ResourceHandle.h"
#include "Bang/Serializable.h"
#include "Bang/String.h"

namespace Bang
{
class Animation;
class AnimatorStateMachine;
class AnimatorStateMachineLayer;
class AnimatorStateMachineTransition;
class IEventsAnimatorStateMachineNode;
class IEventsDestroy;

class AnimatorStateMachineNode
    : public Serializable,
      public EventEmitter<IEventsDestroy>,
      public EventEmitter<IEventsAnimatorStateMachineNode>
{
    SERIALIZABLE(AnimatorStateMachineNode);

public:
    AnimatorStateMachineNode();
    virtual ~AnimatorStateMachineNode() override;

    void SetName(const String &name);
    AnimatorStateMachineTransition *CreateTransitionTo(
        AnimatorStateMachineNode *nodeTo);
    const AnimatorStateMachineTransition *GetTransition(
        uint transitionIdx) const;
    AnimatorStateMachineTransition *GetTransition(uint transitionIdx);
    void RemoveTransition(AnimatorStateMachineTransition *transition);

    void SetAnimation(Animation *animation);

    const String &GetName() const;
    Animation *GetAnimation() const;
    Array<AnimatorStateMachineTransition *> GetTransitionsTo(
        AnimatorStateMachineNode *nodeTo) const;
    const Array<AnimatorStateMachineTransition *> &GetTransitions() const;

    AnimatorStateMachine *GetStateMachine() const;
    AnimatorStateMachineLayer *GetLayer() const;

    // ICloneable
    void CloneInto(ICloneable *clone) const override;

    // Serializable
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;

private:
    String m_name = "Node";
    RH<Animation> p_animation;
    AnimatorStateMachineLayer *p_layer = nullptr;
    Array<AnimatorStateMachineTransition *> m_transitions;

    void SetLayer(AnimatorStateMachineLayer *stateMachineLayer);

    AnimatorStateMachineTransition *AddTransition(
        AnimatorStateMachineTransition *connection);

    friend class AnimatorStateMachineLayer;
};
}

#endif  // ANIMATORSTATEMACHINENODE_H
