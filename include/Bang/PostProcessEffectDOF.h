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

    void SetNearFadingSlope(float nearFadingSlope);
    void SetNearDistance(float nearDistance);
    void SetNearFadingSize(float nearFadingSize);
    void SetFarFadingSlope(float farFadingSlope);
    void SetFarDistance(float farDistance);
    void SetFarFadingSize(float farFadingSize);
    void SetBlurRadius(uint blurRadius);

    float GetNearFadingSlope() const;
    float GetNearDistance() const;
    float GetNearFadingSize() const;
    float GetFarFadingSlope() const;
    float GetFarDistance() const;
    float GetFarFadingSize() const;
    uint GetBlurRadius() const;

    // IReflectable
    void Reflect() override;

private:
    AH<ShaderProgram> m_dofSP;
    AH<Texture2D> m_blurredTexture;
    AH<Texture2D> m_blurAuxiliarTexture;

    float m_nearFadingSlope = 1.0f;
    float m_nearFadingSize = 1.0f;
    float m_nearDistance = 0.5f;
    float m_farFadingSlope = 1.0f;
    float m_farFadingSize = 5.0f;
    float m_farDistance = 10.0f;
    uint m_blurRadius = 3;
};
}

#endif  // POSTPROCESSEFFECTDOF_H
