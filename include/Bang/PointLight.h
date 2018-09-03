#ifndef POINTLIGHT_H
#define POINTLIGHT_H

#include "Bang/Light.h"
#include "Bang/ShaderProgram.h"
#include "Bang/TextureCubeMap.h"

NAMESPACE_BANG_BEGIN

FORWARD class Camera;
FORWARD class Framebuffer;

class PointLight : public Light
{
    COMPONENT(PointLight)

public:
    void SetRange(float range);
    float GetRange() const;

    // Light
    TextureCubeMap* GetShadowMapTexture() const override;

    // ICloneable
    virtual void CloneInto(ICloneable *clone) const override;

    // Serializable
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;

protected:
    float m_range = 1.0f;
    RH<Material> m_shadowMapMaterial;
    Framebuffer *m_shadowMapFramebuffer = nullptr;

    PointLight();
    virtual ~PointLight();

    float GetLightZFar() const;

    // Light
    void RenderShadowMaps_() override;
    Array<Matrix4> GetWorldToShadowMapMatrices() const;
    virtual void SetUniformsBeforeApplyingLight(ShaderProgram* sp) const override;

    // Renderer
    AARect GetRenderRect(Camera *cam) const override;
};

NAMESPACE_BANG_END

#endif // POINTLIGHT_H
