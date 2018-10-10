#ifndef DIRECTIONALLIGHT_H
#define DIRECTIONALLIGHT_H

#include "Bang/Light.h"
#include "Bang/Matrix4.h"
#include "Bang/Texture2D.h"

NAMESPACE_BANG_BEGIN

FORWARD class Scene;
FORWARD class Framebuffer;

class DirectionalLight : public Light
{
    COMPONENT_WITH_FAST_DYNAMIC_CAST(DirectionalLight)

public:
    void SetShadowDistance(float shadowDistance);
    float GetShadowDistance() const;

    // Light
    Texture2D* GetShadowMapTexture() const override;

    // Light
    void SetUniformsBeforeApplyingLight(ShaderProgram *sp) const override;

    // ICloneable
    virtual void CloneInto(ICloneable *clone) const override;

    // Serializable
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;

protected:
    Framebuffer *m_shadowMapFramebuffer = nullptr;
    Matrix4 m_lastUsedShadowMapViewProj = Matrix4::Identity;
    float m_shadowDistance = 100.0f;

    DirectionalLight();
    virtual ~DirectionalLight();

    AABox GetShadowCastersAABox(
            const Array<Renderer*> &shadowCastersRenderers) const;

    // Light
    void RenderShadowMaps_(GameObject *go) override;

    void GetWorldToShadowMapMatrices(
                        Matrix4 *viewMatrix,
                        Matrix4 *projMatrix,
                        const Array<Renderer*> &shadowCastersRenderers) const;
    Matrix4 GetLightToWorldMatrix() const;
    AABox GetShadowMapOrthoBox(
                        const Array<Renderer*> &shadowCastersRenderers) const;
};

NAMESPACE_BANG_END

#endif // DIRECTIONALLIGHT_H
