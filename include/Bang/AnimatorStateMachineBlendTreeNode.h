#ifndef ANIMATORSTATEMACHINEBLENDTREENODE_H
#define ANIMATORSTATEMACHINEBLENDTREENODE_H

#include "Bang/AnimatorStateMachineNode.h"

namespace Bang
{
class Animator;
class AnimatorStateMachineBlendTreeNode : public AnimatorStateMachineNode
{
    SERIALIZABLE(AnimatorStateMachineBlendTreeNode);

public:
    AnimatorStateMachineBlendTreeNode();
    virtual ~AnimatorStateMachineBlendTreeNode();

    void SetSecondAnimation(Animation *secondAnimation);
    void SetSecondAnimationSpeed(float secondAnimationSpeed);
    void SetBlendVariableName(const String &blendVariableName);

    virtual Map<String, Transformation> GetBoneTransformations(
        Time animationTime,
        Animator *animator) const override;
    Animation *GetSecondAnimation() const;
    const String &GetBlendVariableName() const;
    float GetSecondAnimationSpeed() const;

    // ICloneable
    virtual void CloneInto(ICloneable *clone, bool cloneGUID) const override;

    // Serializable
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;

private:
    float m_secondAnimationSpeed = 1.0f;
    AH<Animation> p_secondAnimation;
    String m_blendVariableName = "";
};
}

#endif  // ANIMATORSTATEMACHINEBLENDTREENODE_H
