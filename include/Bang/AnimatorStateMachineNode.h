#ifndef ANIMATORSTATEMACHINENODE_H
#define ANIMATORSTATEMACHINENODE_H

#include <vector>

#include "Bang/Animation.h"
#include "Bang/Array.h"
#include "Bang/Array.tcc"
#include "Bang/AssetHandle.h"
#include "Bang/BangDefines.h"
#include "Bang/DPtr.h"
#include "Bang/EventEmitter.h"
#include "Bang/EventEmitter.tcc"
#include "Bang/EventListener.tcc"
#include "Bang/IEvents.h"
#include "Bang/IEventsAnimatorStateMachineNode.h"
#include "Bang/IEventsDestroy.h"
#include "Bang/MetaNode.h"
#include "Bang/Serializable.h"
#include "Bang/String.h"

namespace Bang
{
class Animator;
class Animation;
class AnimatorStateMachine;
class AnimatorStateMachineLayer;
class AnimatorStateMachineTransition;

class AnimatorStateMachineNode
    : public Serializable,
      public EventEmitterIEventsDestroyWithCheck,
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

    virtual Map<String, Transformation> GetBoneTransformations(
        Time animationTime,
        Animator *animator) const;

    void SetSpeed(float speed);
    void SetAnimation(Animation *animation);

    const String &GetName() const;
    Animation *GetAnimation() const;
    Array<AnimatorStateMachineTransition *> GetTransitionsTo(
        AnimatorStateMachineNode *nodeTo) const;
    const Array<AnimatorStateMachineTransition *> &GetTransitions() const;

    float GetSpeed() const;
    AnimatorStateMachine *GetStateMachine() const;
    AnimatorStateMachineLayer *GetLayer() const;

    // ICloneable
    virtual void CloneInto(ICloneable *clone, bool cloneGUID) const override;

    // Serializable
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;

private:
    String m_name = "Node";
    float m_speed = 1.0f;
    AH<Animation> p_animation;
    AnimatorStateMachineLayer *p_layer = nullptr;
    Array<AnimatorStateMachineTransition *> m_transitions;

    void SetLayer(AnimatorStateMachineLayer *stateMachineLayer);

    AnimatorStateMachineTransition *AddTransition(
        AnimatorStateMachineTransition *connection);

    friend class AnimatorStateMachineLayer;
};
}  // namespace Bang

#endif  // ANIMATORSTATEMACHINENODE_H
