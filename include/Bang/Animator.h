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
    COMPONENT(Animator);

public:
    static constexpr int MaxNumBones = 128;

    Animator();
	virtual ~Animator();

    // Component
    void OnStart() override;
    void OnUpdate() override;
    void OnRender(RenderPass rp) override;

    void SetAnimation(Animation *animation);
    void Play();
    void Stop();
    void Pause();

    bool IsPlaying() const;
    Animation* GetAnimation() const;

    // ICloneable
    virtual void CloneInto(ICloneable *clone) const override;

    // Serializable
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;

private:
    bool m_playing = false;
    float m_animationTimeSeconds = 0.0f;
    Time::TimeT m_prevFrameTimeMillis = 0;

    RH<Animation> p_animation;

    void SetSkinnedMeshRendererCurrentBoneMatrices(
                                const Map<String, Matrix4> &boneAnimMatrices);
};

NAMESPACE_BANG_END

#endif // ANIMATOR_H

