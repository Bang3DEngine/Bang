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
    static Array<Matrix4> s_identityMatrices;
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
    virtual void ImportXML(const XMLNode &xmlInfo) override;
    virtual void ExportXML(XMLNode *xmlInfo) const override;

private:
    bool m_playing = false;
    double m_animationTimeSeconds = 0.0;
    Time::TimeT m_prevFrameTimeMillis = 0;

    RH<Animation> p_animation;

    void SetSkinnedMeshRendererCurrentBoneMatrices(
                                RenderPass rp,
                                const Array<Matrix4> &boneMatrices);
};

NAMESPACE_BANG_END

#endif // ANIMATOR_H

