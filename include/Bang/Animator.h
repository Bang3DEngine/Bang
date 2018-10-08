#ifndef ANIMATOR_H
#define ANIMATOR_H

#include "Bang/Bang.h"
#include "Bang/Time.h"
#include "Bang/Animation.h"
#include "Bang/Component.h"
#include "Bang/EventEmitter.h"
#include "Bang/ResourceHandle.h"
#include "Bang/IEventsAnimator.h"
#include "Bang/AnimatorStateMachine.h"

NAMESPACE_BANG_BEGIN

class Animator : public Component,
                 public EventEmitter<IEventsAnimator>
{
    COMPONENT_WITH_FAST_DYNAMIC_CAST(Animator)

public:
    static constexpr int MaxNumBones = 128;

    Animator();
	virtual ~Animator();

    // Component
    void OnStart() override;
    void OnUpdate() override;

    void SetStateMachine(AnimatorStateMachine *stateMachine);

    void AddAnimation(Animation *animation, uint index = SCAST<uint>(-1));
    void RemoveAnimationByIndex(Animation *animation);
    void RemoveAnimationByIndex(uint animationIndex);
    void SetAnimation(uint animationIndex, Animation *animation);
    void ChangeCurrentAnimation(uint animationIndex);
    void ChangeCurrentAnimationCrossFade(uint animationIndex,
                                         Time crossFadeTime);
    void ChangeCurrentAnimationCrossFade(uint animationIndex,
                                         double crossFadeTimeSeconds);
    void ClearCurrentAnimation();

    void SetPlayOnStart(bool playOnStart);

    void Play();
    void Stop();
    void Pause();

    bool IsPlaying() const;
    bool GetPlayOnStart() const;
    AnimatorStateMachine* GetStateMachine() const;
    Animation* GetAnimation(uint animationIndex) const;
    const Array< RH<Animation> >& GetAnimations() const;

    // ICloneable
    virtual void CloneInto(ICloneable *clone) const override;

    // Serializable
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;

private:
    bool m_playing = false;
    Time m_animationTime;
    Time m_prevFrameTime;

    RH<AnimatorStateMachine> m_stateMachine;

    Array< RH<Animation> > p_animations;
    uint m_currentAnimationIndex = -1u;
    uint m_currentTargetCrossFadeAnimationIndex = -1u;
    Time m_initCrossFadeTime;
    Time m_endCrossFadeTime;
    bool m_playOnStart = true;

    void EndCrossFade();

    void PropagateAnimatorEvents(uint currentAnimationIndex,
                                 Time currentAnimationTime);

    uint GetCurrentAnimationIndex() const;
    Animation *GetCurrentAnimation() const;
    uint GetCurrentTargetCrossFadeAnimationIndex() const;
    Animation *GetCurrentTargetCrossFadeAnimation() const;
    void SetSkinnedMeshRendererCurrentBoneMatrices(
                                const Map<String, Matrix4> &boneAnimMatrices);

    static Map<String, Matrix4> GetBoneAnimationMatrices(Animation *animation,
                                                         Time animationTime);
    static Map<String, Matrix4> GetBoneCrossFadeAnimationMatrices(
                                                Animation *prevAnimation,
                                                Time prevAnimationTime,
                                                Animation *nextAnimation,
                                                Time currentCrossFadeTime,
                                                Time totalCrossFadeTime);
};

NAMESPACE_BANG_END

#endif // ANIMATOR_H

