#ifndef ANIMATOR_H
#define ANIMATOR_H

#include "Bang/Bang.h"
#include "Bang/Time.h"
#include "Bang/Animation.h"
#include "Bang/Component.h"
#include "Bang/ResourceHandle.h"

NAMESPACE_BANG_BEGIN

class Animator : public Component
{
    COMPONENT_WITH_FAST_DYNAMIC_CAST(Animator);

public:
    static constexpr int MaxNumBones = 128;

    Animator();
	virtual ~Animator();

    // Component
    void OnStart() override;
    void OnUpdate() override;

    void AddAnimation(Animation *animation, uint index = SCAST<uint>(-1));
    void RemoveAnimationByIndex(Animation *animation);
    void RemoveAnimationByIndex(uint animationIndex);
    void SetAnimation(uint animationIndex, Animation *animation);
    void ChangeCurrentAnimation(uint animationIndex);
    void ChangeCurrentAnimationCrossFade(uint animationIndex,
                                         double crossFadeTimeSeconds);
    void ClearCurrentAnimation();

    void SetPlayOnStart(bool playOnStart);

    void Play();
    void Stop();
    void Pause();

    bool IsPlaying() const;
    bool GetPlayOnStart() const;
    Animation* GetAnimation(uint animationIndex) const;
    const Array< RH<Animation> >& GetAnimations() const;

    // ICloneable
    virtual void CloneInto(ICloneable *clone) const override;

    // Serializable
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;

private:
    bool m_playing = false;
    float m_animationTimeSeconds = 0.0f;
    Time::TimeT m_prevFrameTimeMillis = 0;

    Array< RH<Animation> > p_animations;
    uint m_currentAnimationIndex = -1u;
    uint m_currentTargetCrossFadeAnimationIndex = -1u;
    double m_initCrossFadeTime = Math::Infinity<double>();
    double m_endCrossFadeTime = Math::Infinity<double>();;
    bool m_playOnStart = true;

    void EndCrossFade();

    uint GetCurrentAnimationIndex() const;
    Animation *GetCurrentAnimation() const;
    uint GetCurrentTargetCrossFadeAnimationIndex() const;
    Animation *GetCurrentTargetCrossFadeAnimation() const;
    void SetSkinnedMeshRendererCurrentBoneMatrices(
                                const Map<String, Matrix4> &boneAnimMatrices);

    static Map<String, Matrix4> GetBoneAnimationMatrices(Animation *animation,
                                                         double animationSeconds);
    static Map<String, Matrix4> GetBoneCrossFadeAnimationMatrices(
                                                Animation *prevAnimation,
                                                double prevAnimationSeconds,
                                                Animation *nextAnimation,
                                                double currentCrossFadeSeconds,
                                                double totalCrossFadeSeconds);
};

NAMESPACE_BANG_END

#endif // ANIMATOR_H

