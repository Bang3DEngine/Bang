#ifndef POSTPROCESSEFFECTBLOOM_H
#define POSTPROCESSEFFECTBLOOM_H

#include "Bang/PostProcessEffect.h"

namespace Bang
{
class Framebuffer;

class PostProcessEffectBloom : public PostProcessEffect
{
    COMPONENT_WITH_FAST_DYNAMIC_CAST(PostProcessEffectBloom)

public:
    PostProcessEffectBloom();
    virtual ~PostProcessEffectBloom();

    // Component
    virtual void OnRender(RenderPass renderPass) override;

    void SetIntensity(float intensity);
    void SetBlurRadius(uint blurRadius);
    void SetBrightnessThreshold(float brightnessThreshold);

    float GetIntensity() const;
    uint GetBlurRadius() const;
    float GetBrightnessThreshold() const;
    ShaderProgram *GetBloomShaderProgram() const;

    // IReflectable
    void Reflect() override;

private:
    Framebuffer *m_bloomFramebuffer = nullptr;
    float m_brightnessThreshold = 1.0f;
    float m_intensity = 1.0f;
    uint m_blurRadius = 5;

    RH<ShaderProgram> p_bloomSP;
    RH<Texture2D> m_brightnessTexture;
    RH<Texture2D> m_blurredBloomTexture;
    RH<Texture2D> m_blurAuxiliarTexture;
};
}

#endif  // POSTPROCESSEFFECTBLOOM_H
