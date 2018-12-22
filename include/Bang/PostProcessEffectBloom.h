#ifndef POSTPROCESSEFFECTBLOOM_H
#define POSTPROCESSEFFECTBLOOM_H

#include "Bang/PostProcessEffect.h"

namespace Bang
{
class Framebuffer;

class PostProcessEffectBloom : public PostProcessEffect
{
    COMPONENT(PostProcessEffectBloom)

public:
    PostProcessEffectBloom();
    virtual ~PostProcessEffectBloom();

    // Component
    virtual void OnRender(RenderPass renderPass) override;

    void SetDownscale(uint downscale);
    void SetIntensity(float intensity);
    void SetBlurRadius(uint blurRadius);
    void SetUseKawaseBlur(bool useKawaseBlur);
    void SetBrightnessThreshold(float brightnessThreshold);
    void SetUseHighBitDepthTextures(bool useHighBitDepthTextures);

    uint GetDownscale() const;
    float GetIntensity() const;
    uint GetBlurRadius() const;
    bool GetUseKawaseBlur() const;
    float GetBrightnessThreshold() const;
    Texture2D *GetFinalBloomTexture() const;
    bool GetUseHighBitDepthTextures() const;
    ShaderProgram *GetBloomShaderProgram() const;

    // IReflectable
    void Reflect() override;

private:
    Framebuffer *m_bloomFramebuffer = nullptr;
    bool m_useHighBitDepthTextures = true;
    float m_brightnessThreshold = 1.0f;
    bool m_useKawaseBlur = true;
    float m_intensity = 1.0f;
    uint m_blurRadius = 5;
    uint m_downscale = 2;

    AH<ShaderProgram> p_bloomSP;
    AH<Texture2D> m_brightnessTexture;
    AH<Texture2D> m_blurredBloomTexture;
    AH<Texture2D> m_blurAuxiliarTexture;
};
}

#endif  // POSTPROCESSEFFECTBLOOM_H
