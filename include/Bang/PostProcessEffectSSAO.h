#ifndef POSTPROCESSEFFECTSSAO_H
#define POSTPROCESSEFFECTSSAO_H

#include "Bang/Bang.h"
#include "Bang/ShaderProgram.h"
#include "Bang/ResourceHandle.h"
#include "Bang/PostProcessEffect.h"

NAMESPACE_BANG_BEGIN

FORWARD class Framebuffer;

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
    void SetNumRandomRotations(int numRotations);
    void SetBilateralBlurEnabled(bool bilateralBlurEnabled);
    void SetFBSize(const Vector2 &fbSize);

    int GetBlurRadius() const;
    float GetSSAORadius() const;
    float GetSSAOIntensity() const;
    int GetNumRandomSamples() const;
    int GetNumRandomRotations() const;
    bool GetBilateralBlurEnabled() const;
    const Vector2 &GetFBSize() const;
    Texture2D* GetSSAOTexture() const;

private:
    float m_ssaoIntensity = 1.0f;
    float m_ssaoRadius = -1;
    int m_blurRadius = -1;
    int m_numRotations = -1;
    int m_numRandomOffsetsHemisphere = -1;
    bool m_bilateralBlurEnabled = true;
    Vector2 m_fbSize = Vector2::One;

    Array<float> m_blurKernel;
    Array<Vector3> m_randomHemisphereOffsets;
    RH<Texture2D> m_randomRotationsTexture;

    Framebuffer *m_ssaoFB = nullptr;
    RH<ShaderProgram> p_ssaoShaderProgram;
    RH<ShaderProgram> p_blurXShaderProgram;
    RH<ShaderProgram> p_blurYShaderProgram;
    RH<ShaderProgram> p_applySSAOShaderProgram;

    void GenerateRandomRotationsTexture(int numRotations);

	PostProcessEffectSSAO();
	virtual ~PostProcessEffectSSAO();
};

NAMESPACE_BANG_END

#endif // POSTPROCESSEFFECTSSAO_H

