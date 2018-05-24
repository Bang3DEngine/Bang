#ifndef CAMERA_H
#define CAMERA_H

#include "Bang/Ray.h"
#include "Bang/Set.h"
#include "Bang/Color.h"
#include "Bang/AARect.h"
#include "Bang/Component.h"
#include "Bang/ResourceHandle.h"
#include "Bang/TextureCubeMap.h"

NAMESPACE_BANG_BEGIN

FORWARD class Quad;
FORWARD class GBuffer;
FORWARD class Texture2D;
FORWARD class ShaderProgram;
FORWARD class SelectionFramebuffer;

class Camera : public Component,
               public IDestroyListener
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

    void BindGBuffer();
    void BindSelectionFramebuffer();
    void BindViewportForBlitting() const;
    void BindViewportForRendering() const;

    Ray      FromViewportPointNDCToRay(const Vector2 &vpPointNDC) const;
    Vector2i FromWindowPointToViewportPoint(const Vector2i &winPoint) const;
    Vector3  FromWorldPointToViewportPointNDC(const Vector3 &worldPosition) const;
    Vector3  FromViewportPointNDCToWorldPoint(const Vector3 &vpPositionNDC) const;
    Vector3  FromViewportPointNDCToWorldPoint(const Vector2 &vpPositionNDC,
                                              float zFromCam) const;

    void SetOrthoHeight(float orthoHeight);
    void SetClearColor(const Color& color);
    void SetFovDegrees(float fovDegrees);
    void SetZNear(float zNear);
    void SetZFar(float zFar);
    void SetProjectionMode(ProjectionMode projMode);
    void SetViewportAARectNDC(const AARect &viewportRectNDC);
    void AddRenderPass(RenderPass renderPass);
    void RemoveRenderPass(RenderPass renderPass);
    void SetSkyBoxTexture(TextureCubeMap *skyBoxTextureCM,
                          bool createFilteredCubeMapsForIBL = true);
    void SetRenderSelectionBuffer(bool renderSelectionBuffer);
    void SetClearMode(ClearMode clearMode);

    const Color& GetClearColor() const;
    float GetOrthoWidth() const;
    float GetOrthoHeight() const;
    float GetFovDegrees() const;
    float GetZNear() const;
    float GetZFar() const;
    ClearMode GetClearMode() const;
    bool MustRenderPass(RenderPass renderPass) const;
    const Set<RenderPass>& GetRenderPassMask() const;
    Matrix4 GetViewMatrix() const;
    bool GetRenderSelectionBuffer() const;
    Matrix4 GetProjectionMatrix() const;
    bool IsPointInsideFrustum(const Vector3 &worldPoint) const;
    ProjectionMode GetProjectionMode() const;
    AARect GetViewportBoundingAARectNDC(const AABox &bbox) const;
    AARect GetViewportAARectInWindow() const;
    AARect GetViewportAARectNDCInWindow() const;
    const AARect& GetViewportAARectNDC() const;
    GBuffer *GetGBuffer() const;
    TextureCubeMap *GetSkyBoxTexture() const;
    TextureCubeMap *GetSpecularSkyBoxTexture() const;
    TextureCubeMap *GetDiffuseSkyBoxTexture() const;
    SelectionFramebuffer *GetSelectionFramebuffer() const;

    Quad GetFrustumNearQuad()  const;
    Quad GetFrustumFarQuad()   const;
    Quad GetFrustumLeftQuad()  const;
    Quad GetFrustumRightQuad() const;
    Quad GetFrustumTopQuad()   const;
    Quad GetFrustumBotQuad()   const;

    static Camera *GetActive();

    // Component
    void OnRender(RenderPass rp) override;

    // ICloneable
    virtual void CloneInto(ICloneable *clone) const override;

    // Serializable
    virtual void ImportXML(const XMLNode &xmlInfo) override;
    virtual void ExportXML(XMLNode *xmlInfo) const override;

    // IDestroyListener
    virtual void OnDestroyed(EventEmitter<IDestroyListener> *object) override;

protected:
    Camera();
    virtual ~Camera();

private:
    GBuffer *m_gbuffer = nullptr;
    SelectionFramebuffer *m_selectionFramebuffer = nullptr;

    Set<RenderPass> m_renderPassMask;
    bool m_renderSelectionBuffer = false;
    RH<TextureCubeMap> p_skyboxTextureCM;
    RH<TextureCubeMap> p_skyboxSpecularTextureCM;
    RH<TextureCubeMap> p_skyboxDiffuseTextureCM;

    Color m_clearColor = Color(Color(0.3f), 1);
    ClearMode m_clearMode = ClearMode::COLOR;
    float m_orthoHeight  = 25.0f;
    float m_fovDegrees = 60.0f;
    float m_zNear = 0.1f;
    float m_zFar = 100.0f;
    AARect m_viewportRectNDC = AARect::NDCRect;
    ProjectionMode m_projMode = ProjectionMode::PERSPECTIVE;

    friend class Scene;
};

NAMESPACE_BANG_END

#endif // CAMERA_H
