#ifndef POSTPROCESSEFFECTSSAO_H
#define POSTPROCESSEFFECTSSAO_H

#include "Bang/Array.h"
#include "Bang/Bang.h"
#include "Bang/BangDefines.h"
#include "Bang/ComponentClassIds.h"
#include "Bang/ComponentMacros.h"
#include "Bang/MetaNode.h"
#include "Bang/PostProcessEffect.h"
#include "Bang/RenderPass.h"
#include "Bang/ResourceHandle.h"
#include "Bang/ShaderProgram.h"
#include "Bang/String.h"

NAMESPACE_BANG_BEGIN

FORWARD class ICloneable;
FORWARD class Framebuffer;
FORWARD class ShaderProgram;
FORWARD class Texture2D;

class PostProcessEffectSSAO : public PostProcessEffect
{
    COMPONENT_WITH_FAST_DYNAMIC_CAST(PostProcessEffectSSAO);

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
    Texture2D* GetSSAOTexture() const;

    // ICloneable
    virtual void CloneInto(ICloneable *clone) const override;

    // Serializable
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;

private:
    float m_ssaoIntensity = 1.0f;
    float m_ssaoRadius = -1;
    int m_blurRadius = -1;
    int m_numAxes = -1;
    bool m_separable = false;
    int m_numRandomOffsetsHemisphere = -1;
    bool m_bilateralBlurEnabled = true;
    Vector2 m_fbSize = Vector2::One;

    Array<float> m_blurKernel;
    Array<Vector3> m_randomHemisphereOffsets;
    RH<Texture2D> m_randomAxesTexture;

    Framebuffer *m_ssaoFB = nullptr;
    RH<ShaderProgram> p_ssaoShaderProgram;
    RH<ShaderProgram> p_blurXShaderProgram;
    RH<ShaderProgram> p_blurYShaderProgram;
    RH<ShaderProgram> p_applySSAOShaderProgram;

    void GenerateRandomAxesTexture(int numAxes);

	PostProcessEffectSSAO();
	virtual ~PostProcessEffectSSAO();
};

NAMESPACE_BANG_END

#endif // POSTPROCESSEFFECTSSAO_H

