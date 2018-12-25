#ifndef POINTLIGHT_H
#define POINTLIGHT_H

#include "Bang/Array.h"
#include "Bang/AssetHandle.h"
#include "Bang/BangDefines.h"
#include "Bang/ComponentMacros.h"
#include "Bang/Light.h"
#include "Bang/MetaNode.h"
#include "Bang/String.h"
#include "Bang/TextureCubeMap.h"

namespace Bang
{
class Camera;
class Framebuffer;
class GameObject;
class ICloneable;
class Material;
class ShaderProgram;

class PointLight : public Light
{
    COMPONENT(PointLight)

public:
    void SetRange(float range);
    void SetShadowNearPlane(float nearPlane);

    float GetRange() const;
    float GetShadowMapNearDistance() const override;
    float GetShadowMapFarDistance() const override;

    // Light
    TextureCubeMap *GetShadowMapTexture() const override;

    // Serializable
    void Reflect() override;

protected:
    float m_range = 1.0f;
    float m_shadowNearPlane = 0.1f;
    Framebuffer *m_shadowMapFramebuffer = nullptr;
    AH<TextureCubeMap> m_blurAuxiliarShadowMapTexCM;
    AH<TextureCubeMap> m_blurredShadowMapTexCM;

    PointLight();
    virtual ~PointLight() override;

    float GetLightZFar() const;

    // Light
    void RenderShadowMaps_(GameObject *go) override;
    Array<Matrix4> GetWorldToShadowMapMatrices() const;
    virtual void SetUniformsBeforeApplyingLight(
        ShaderProgram *sp) const override;

    // Renderer
    AARect GetRenderRect(Camera *cam) const override;
};
}

#endif  // POINTLIGHT_H
