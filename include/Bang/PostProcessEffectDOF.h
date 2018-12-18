#ifndef POSTPROCESSEFFECTDOF_H
#define POSTPROCESSEFFECTDOF_H

#include "Bang/PostProcessEffect.h"

namespace Bang
{
class PostProcessEffectDOF : public PostProcessEffect
{
    COMPONENT_WITH_FAST_DYNAMIC_CAST(PostProcessEffectDOF)

public:
    PostProcessEffectDOF();
    virtual ~PostProcessEffectDOF();

    // Component
    virtual void OnRender(RenderPass renderPass) override;

    void SetBlurRadius(uint blurRadius);
    void SetFadeRange(float fadeRange);
    void SetDownscale(uint downscale);
    void SetForegroundThreshold(float foregroundThreshold);

    uint GetBlurRadius() const;
    float GetFadeRange() const;
    float GetForegroundThreshold() const;

    // IReflectable
    void Reflect() override;

private:
    RH<ShaderProgram> m_dofSP;
    RH<Texture2D> m_foregroundTexture;
    RH<Texture2D> m_auxiliarTexture;

    float m_fadeRange = 0.125f;
    float m_foregroundThreshold = 0.5f;
    uint m_blurRadius = 6;
};
}

#endif  // POSTPROCESSEFFECTDOF_H
