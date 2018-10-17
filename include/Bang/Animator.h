#ifndef ANIMATOR_H
#define ANIMATOR_H

#include "Bang/BangDefines.h"
#include "Bang/Component.h"
#include "Bang/ComponentClassIds.h"
#include "Bang/ComponentMacros.h"
#include "Bang/Map.h"
#include "Bang/MetaNode.h"
#include "Bang/ResourceHandle.h"
#include "Bang/String.h"
#include "Bang/Time.h"

NAMESPACE_BANG_BEGIN

FORWARD class Animation;
FORWARD class AnimatorStateMachine;
FORWARD class AnimatorStateMachinePlayer;
FORWARD class ICloneable;

class Animator : public Component
{
    COMPONENT_WITH_FAST_DYNAMIC_CAST(Animator)

public:
    static constexpr int MaxNumBones = 128;

    Animator();
	virtual ~Animator() override;

    // Component
    void OnStart() override;
    void OnUpdate() override;

    void SetStateMachine(AnimatorStateMachine *stateMachine);

    void SetPlayOnStart(bool playOnStart);

    void Play();
    void Stop();
    void Pause();

    bool IsPlaying() const;
    bool GetPlayOnStart() const;
    AnimatorStateMachinePlayer *GetPlayer() const;
    AnimatorStateMachine* GetStateMachine() const;

    // ICloneable
    virtual void CloneInto(ICloneable *clone) const override;

    // Serializable
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;

private:
    Time m_animationTime;
    Time m_prevFrameTime;

    AnimatorStateMachinePlayer *m_animationStateMachinePlayer = nullptr;
    RH<AnimatorStateMachine> m_stateMachine;
    bool m_playOnStart = true;
    bool m_playing = false;


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

