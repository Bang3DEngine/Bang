#ifndef POSTPROCESSEFFECTDOF_H
#define POSTPROCESSEFFECTDOF_H

#include "Bang/PostProcessEffect.h"

namespace Bang
{
class PostProcessEffectDOF : public PostProcessEffect
{
    COMPONENT(PostProcessEffectDOF)

public:
    PostProcessEffectDOF();
    virtual ~PostProcessEffectDOF();

    // Component
    virtual void OnRender(RenderPass renderPass) override;

    void SetFading(float fading);
    void SetBlurRadius(uint blurRadius);
    void SetFocusRange(float focusRange);
    void SetDownscale(uint downscale);
    void SetFocusDistanceWorld(float focusDistanceWorld);

    float GetFading() const;
    uint GetBlurRadius() const;
    float GetFocusRange() const;
    float GetFocusDistanceWorld() const;

    // IReflectable
    void Reflect() override;

private:
    AH<ShaderProgram> m_dofSP;
    AH<Texture2D> m_blurredTexture;
    AH<Texture2D> m_blurAuxiliarTexture;

    float m_fading = 0.1f;
    float m_focusRange = 3.0f;
    float m_focusDistanceWorld = 5.0f;
    uint m_blurRadius = 6;
};
}

#endif  // POSTPROCESSEFFECTDOF_H
