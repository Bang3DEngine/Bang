#ifndef CAMERA_H
#define CAMERA_H

#include <vector>

#include "Bang/Array.tcc"
#include "Bang/BangDefines.h"
#include "Bang/Color.h"
#include "Bang/Component.h"
#include "Bang/ComponentClassIds.h"
#include "Bang/ComponentMacros.h"
#include "Bang/EventEmitter.tcc"
#include "Bang/EventListener.h"
#include "Bang/IEvents.h"
#include "Bang/MetaNode.h"
#include "Bang/Ray.h"
#include "Bang/RenderFlags.h"
#include "Bang/RenderPass.h"
#include "Bang/ResourceHandle.h"
#include "Bang/String.h"
#include "Bang/USet.h"

NAMESPACE_BANG_BEGIN

FORWARD_T class EventEmitter;
FORWARD   class AABox;
FORWARD   class GBuffer;
FORWARD   class ICloneable;
FORWARD   class IEventsDestroy;
FORWARD   class Quad;
FORWARD   class TextureCubeMap;

enum class CameraProjectionMode
{
    ORTHOGRAPHIC,
    PERSPECTIVE
};

enum class CameraClearMode
{
    COLOR,
    SKY_BOX
};

class Camera : public Component,
               public EventListener<IEventsDestroy>
{
    COMPONENT_WITH_FAST_DYNAMIC_CAST(Camera)

public:
    virtual void Bind() const;
    virtual void UnBind() const;

    Ray      FromViewportPointNDCToRay(const Vector2 &vpPointNDC) const;
    Vector3  FromWorldPointToViewportPointNDC(const Vector3 &worldPosition) const;
    Vector3  FromViewportPointNDCToWorldPoint(const Vector3 &vpPositionNDC) const;
    Vector3  FromViewportPointNDCToWorldPoint(const Vector2 &vpPositionNDC,
                                              float zFromCam) const;

    void SetReplacementGBuffer(GBuffer *gbuffer);
    void SetRenderFlags(RenderFlags renderFlags);
    void SetRenderSize(const Vector2i &renderSize);
    void SetGammaCorrection(float gammaCorrection);
    void SetOrthoHeight(float orthoHeight);
    void SetClearColor(const Color& color);
    void SetFovDegrees(float fovDegrees);
    void SetZNear(float zNear);
    void SetZFar(float zFar);
    void SetProjectionMode(CameraProjectionMode projMode);
    void AddRenderPass(RenderPass renderPass);
    void RemoveRenderPass(RenderPass renderPass);
    void SetSkyBoxTexture(TextureCubeMap *skyBoxTextureCM,
                          bool createFilteredCubeMapsForIBL = true);
    void SetClearMode(CameraClearMode clearMode);

    const Color& GetClearColor() const;
    float GetAspectRatio() const;
    float GetOrthoWidth() const;
    float GetOrthoHeight() const;
    float GetFovDegrees() const;
    float GetZNear() const;
    float GetZFar() const;
    CameraClearMode GetClearMode() const;
    float GetGammaCorrection() const;
    RenderFlags GetRenderFlags() const;
    bool MustRenderPass(RenderPass renderPass) const;
    const USet<RenderPass, EnumClassHash>& GetRenderPassMask() const;
    Matrix4 GetViewMatrix() const;
    Matrix4 GetProjectionMatrix() const;
    bool IsPointInsideFrustum(const Vector3 &worldPoint) const;
    CameraProjectionMode GetProjectionMode() const;
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
    virtual ~Camera() override;

private:
    GBuffer *m_gbuffer = nullptr;
    GBuffer *p_replacementGBuffer = nullptr;

    RenderFlags m_renderFlags = RenderFlag::DEFAULT;
    USet<RenderPass, EnumClassHash> m_renderPassMask;
    RH<TextureCubeMap> p_skyboxTextureCM;
    RH<TextureCubeMap> p_skyboxSpecularTextureCM;
    RH<TextureCubeMap> p_skyboxDiffuseTextureCM;

    CameraProjectionMode m_projMode = CameraProjectionMode::PERSPECTIVE;
    CameraClearMode m_clearMode = CameraClearMode::COLOR;
    Color m_clearColor = Color(Color(0.3f), 1);
    float m_orthoHeight  = 25.0f;
    float m_fovDegrees = 60.0f;
    float m_zNear = 0.1f;
    float m_zFar = 100.0f;
    float m_gammaCorrection = 1.0f / 2.2f;

    friend class Scene;
};

NAMESPACE_BANG_END

#endif // CAMERA_H
