#ifndef POSTPROCESSEFFECTTONEMAPPING_H
#define POSTPROCESSEFFECTTONEMAPPING_H

#include "Bang/PostProcessEffect.h"

namespace Bang
{
class PostProcessEffectToneMapping : public PostProcessEffect
{
    COMPONENT(PostProcessEffectToneMapping)

public:
    PostProcessEffectToneMapping();
    virtual ~PostProcessEffectToneMapping();

    // Component
    virtual void OnRender(RenderPass renderPass) override;

    void SetExposure(float exposure);

    float GetExposure() const;

    // IReflectable
    void Reflect() override;

private:
    AH<ShaderProgram> m_toneMappingShaderProgram;

    float m_exposure = 1.0f;
};
}

#endif  // POSTPROCESSEFFECTTONEMAPPING_H
