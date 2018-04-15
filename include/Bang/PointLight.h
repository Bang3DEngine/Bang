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

    // Component
    void OnRender(RenderPass rp) override;

    // ICloneable
    virtual void CloneInto(ICloneable *clone) const override;

    // Serializable
    virtual void ImportXML(const XMLNode &xmlInfo) override;
    virtual void ExportXML(XMLNode *xmlInfo) const override;

protected:
    float m_range = 1.0f;

    RH<ShaderProgram> m_shadowMapShaderProgram;
    Framebuffer *m_shadowMapFramebuffer = nullptr;
    TextureCubeMap *m_shadowMapTexCubeMap = nullptr;

    PointLight();
    virtual ~PointLight();

    // Light
    void RenderShadowMaps_() override;
    virtual void SetUniformsBeforeApplyingLight(Material* mat) const override;

    // Renderer
    AARect GetRenderRect(Camera *cam) const override;
};

NAMESPACE_BANG_END

#endif // POINTLIGHT_H
