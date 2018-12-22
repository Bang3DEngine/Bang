#ifndef DIRECTIONALLIGHT_H
#define DIRECTIONALLIGHT_H

#include "Bang/AABox.h"
#include "Bang/Array.h"
#include "Bang/BangDefines.h"
#include "Bang/ComponentMacros.h"
#include "Bang/Light.h"
#include "Bang/Matrix4.h"
#include "Bang/MetaNode.h"
#include "Bang/String.h"
#include "Bang/Texture2D.h"

namespace Bang
{
class Framebuffer;
class GameObject;
class ICloneable;
class Renderer;
class ShaderProgram;

class DirectionalLight : public Light
{
    COMPONENT(DirectionalLight)

public:
    void SetShadowDistance(float shadowDistance);

    float GetShadowMapNearDistance() const override;
    float GetShadowMapFarDistance() const override;
    float GetShadowDistance() const;

    // Light
    Texture2D *GetShadowMapTexture() const override;

    // Light
    void SetUniformsBeforeApplyingLight(ShaderProgram *sp) const override;

    // Serializable
    void Reflect() override;

protected:
    AH<Texture2D> m_blurredShadowMapTexture, m_blurAuxiliarTexture;
    Framebuffer *m_shadowMapFramebuffer = nullptr;
    Matrix4 m_lastUsedShadowMapViewProj = Matrix4::Identity();
    float m_shadowDistance = 100.0f;

    DirectionalLight();
    virtual ~DirectionalLight() override;

    AABox GetShadowCastersAABox(
        const Array<Renderer *> &shadowCastersRenderers) const;

    // Light
    void RenderShadowMaps_(GameObject *go) override;

    void GetWorldToShadowMapMatrices(
        Matrix4 *viewMatrix,
        Matrix4 *projMatrix,
        const Array<Renderer *> &shadowCastersRenderers) const;
    Matrix4 GetLightToWorldMatrix() const;
    AABox GetShadowMapOrthoBox(
        const Array<Renderer *> &shadowCastersRenderers) const;
};
}

#endif  // DIRECTIONALLIGHT_H
