#ifndef POSTPROCESSEFFECTFXAA_H
#define POSTPROCESSEFFECTFXAA_H

#include "Bang/PostProcessEffect.h"

namespace Bang
{
class PostProcessEffectFXAA : public PostProcessEffect
{
    COMPONENT_WITH_FAST_DYNAMIC_CAST(PostProcessEffectFXAA)

public:
    PostProcessEffectFXAA();
    virtual ~PostProcessEffectFXAA() override;

    // Component
    virtual void OnRender(RenderPass renderPass) override;

    // IReflectable
    void Reflect() override;

private:
    RH<ShaderProgram> m_fxaaShaderProgram;
};
}

#endif  // POSTPROCESSEFFECTFXAA_H
