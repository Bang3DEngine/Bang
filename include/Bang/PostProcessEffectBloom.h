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

    ShaderProgram *GetBloomShaderProgram() const;

private:
    Framebuffer *m_bloomFramebuffer = nullptr;

    RH<ShaderProgram> p_bloomSP;
    RH<Texture2D> m_blurredBloomTexture, m_blurAuxiliarTexture;
};
}

#endif  // POSTPROCESSEFFECTBLOOM_H
