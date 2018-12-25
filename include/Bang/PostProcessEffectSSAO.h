#ifndef POSTPROCESSEFFECTSSAO_H
#define POSTPROCESSEFFECTSSAO_H

#include "Bang/Array.h"
#include "Bang/AssetHandle.h"
#include "Bang/BangDefines.h"
#include "Bang/ComponentMacros.h"
#include "Bang/MetaNode.h"
#include "Bang/PostProcessEffect.h"
#include "Bang/RenderPass.h"
#include "Bang/String.h"

namespace Bang
{
class Framebuffer;
class ICloneable;
class ShaderProgram;
class Texture2D;

class PostProcessEffectSSAO : public PostProcessEffect
{
    COMPONENT(PostProcessEffectSSAO);

public:
    // Component
    virtual void OnRender(RenderPass renderPass) override;

    void SetSSAORadius(float radius);
    void SetSSAOIntensity(float ssaoIntensity);
    void SetBlurRadius(int blurRadius);
    void SetNumRandomSamples(int numRandomSamples);
    void SetNumRandomAxes(int numAxes);
    void SetSeparable(bool separable);
    void SetBilateralBlurEnabled(bool bilateralBlurEnabled);
    void SetFBSize(const Vector2 &fbSize);

    bool GetSeparable() const;
    int GetBlurRadius() const;
    float GetSSAORadius() const;
    float GetSSAOIntensity() const;
    int GetNumRandomSamples() const;
    int GetNumRandomAxes() const;
    bool GetBilateralBlurEnabled() const;
    const Vector2 &GetFBSize() const;
    Texture2D *GetSSAOTexture() const;

    // IReflectable
    virtual void Reflect() override;

private:
    float m_ssaoIntensity = 1.0f;
    float m_ssaoRadius = -1;
    uint m_blurRadius = 4;
    int m_numAxes = -1;
    bool m_separable = true;
    int m_numRandomOffsetsHemisphere = -1;
    Vector2 m_fbSize = Vector2::One();

    Array<Vector3> m_randomHemisphereOffsets;
    AH<Texture2D> m_randomAxesTexture;

    Framebuffer *m_ssaoFB = nullptr;
    AH<Texture2D> m_blurAuxiliarTexture;
    AH<Texture2D> m_blurredSSAOTexture;
    AH<ShaderProgram> p_ssaoShaderProgram;
    AH<ShaderProgram> p_applySSAOShaderProgram;

    void GenerateRandomAxesTexture(int numAxes);

    PostProcessEffectSSAO();
    virtual ~PostProcessEffectSSAO() override;
};
}

#endif  // POSTPROCESSEFFECTSSAO_H
