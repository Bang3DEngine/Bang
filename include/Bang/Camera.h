#ifndef CAMERA_H
#define CAMERA_H

#include "Bang/Ray.h"
#include "Bang/USet.h"
#include "Bang/Color.h"
#include "Bang/AARect.h"
#include "Bang/Component.h"
#include "Bang/RenderFlags.h"
#include "Bang/ResourceHandle.h"
#include "Bang/TextureCubeMap.h"

NAMESPACE_BANG_BEGIN

FORWARD class Quad;
FORWARD class GBuffer;
FORWARD class Texture2D;
FORWARD class ShaderProgram;

class Camera : public Component,
               public EventListener<IEventsDestroy>
{
    COMPONENT(Camera)

public:
    enum class ProjectionMode
    {
        ORTHOGRAPHIC,
        PERSPECTIVE
    };

    enum class ClearMode
    {
        COLOR,
        SKY_BOX
    };

    virtual void Bind() const;
    virtual void UnBind() const;

    Ray      FromViewportPointNDCToRay(const Vector2 &vpPointNDC) const;
    Vector3  FromWorldPointToViewportPointNDC(const Vector3 &worldPosition) const;
    Vector3  FromViewportPointNDCToWorldPoint(const Vector3 &vpPositionNDC) const;
    Vector3  FromViewportPointNDCToWorldPoint(const Vector2 &vpPositionNDC,
                                              float zFromCam) const;

    void SetRenderFlags(RenderFlags renderFlags);
    void SetRenderSize(const Vector2i &renderSize);
    void SetGammaCorrection(float gammaCorrection);
    void SetOrthoHeight(float orthoHeight);
    void SetClearColor(const Color& color);
    void SetFovDegrees(float fovDegrees);
    void SetZNear(float zNear);
    void SetZFar(float zFar);
    void SetProjectionMode(ProjectionMode projMode);
    void AddRenderPass(RenderPass renderPass);
    void RemoveRenderPass(RenderPass renderPass);
    void SetSkyBoxTexture(TextureCubeMap *skyBoxTextureCM,
                          bool createFilteredCubeMapsForIBL = true);
    void SetClearMode(ClearMode clearMode);

    const Color& GetClearColor() const;
    float GetAspectRatio() const;
    float GetOrthoWidth() const;
    float GetOrthoHeight() const;
    float GetFovDegrees() const;
    float GetZNear() const;
    float GetZFar() const;
    ClearMode GetClearMode() const;
    float GetGammaCorrection() const;
    RenderFlags GetRenderFlags() const;
    bool MustRenderPass(RenderPass renderPass) const;
    const USet<RenderPass, EnumClassHash>& GetRenderPassMask() const;
    Matrix4 GetViewMatrix() const;
    Matrix4 GetProjectionMatrix() const;
    bool IsPointInsideFrustum(const Vector3 &worldPoint) const;
    ProjectionMode GetProjectionMode() const;
    AARect GetViewportBoundingAARectNDC(const AABox &bbox) const;
    GBuffer *GetGBuffer() const;
    const Vector2i &GetRenderSize() const;
    TextureCubeMap *GetSkyBoxTexture() const;
    TextureCubeMap *GetSpecularSkyBoxTexture() const;
    TextureCubeMap *GetDiffuseSkyBoxTexture() const;

    Quad GetFrustumNearQuad()  const;
    Quad GetFrustumFarQuad()   const;
    Quad GetFrustumLeftQuad()  const;
    Quad GetFrustumRightQuad() const;
    Quad GetFrustumTopQuad()   const;
    Quad GetFrustumBotQuad()   const;

    static Camera *GetActive();

    // ICloneable
    virtual void CloneInto(ICloneable *clone) const override;

    // Serializable
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;

    // IEventsDestroy
    virtual void OnDestroyed(EventEmitter<IEventsDestroy> *object) override;

protected:
    Camera();
    virtual ~Camera();

private:
    GBuffer *m_gbuffer = nullptr;

    RenderFlags m_renderFlags = RenderFlag::DEFAULT;
    USet<RenderPass, EnumClassHash> m_renderPassMask;
    RH<TextureCubeMap> p_skyboxTextureCM;
    RH<TextureCubeMap> p_skyboxSpecularTextureCM;
    RH<TextureCubeMap> p_skyboxDiffuseTextureCM;

    Color m_clearColor = Color(Color(0.3f), 1);
    ClearMode m_clearMode = ClearMode::COLOR;
    float m_orthoHeight  = 25.0f;
    float m_fovDegrees = 60.0f;
    float m_zNear = 0.1f;
    float m_zFar = 100.0f;
    float m_gammaCorrection = 1.0f / 2.2f;
    ProjectionMode m_projMode = ProjectionMode::PERSPECTIVE;

    friend class Scene;
};

NAMESPACE_BANG_END

#endif // CAMERA_H
