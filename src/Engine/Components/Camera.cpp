#include "Bang/Camera.h"

#include <list>
#include <unordered_map>

#include "Bang/AABox.h"
#include "Bang/Array.h"
#include "Bang/Assets.h"
#include "Bang/Assets.tcc"
#include "Bang/ClassDB.h"
#include "Bang/CubeMapIBLGenerator.h"
#include "Bang/EventListener.tcc"
#include "Bang/Flags.h"
#include "Bang/GBuffer.h"
#include "Bang/GEngine.h"
#include "Bang/GL.h"
#include "Bang/GLUniforms.h"
#include "Bang/GUID.h"
#include "Bang/GameObject.h"
#include "Bang/Geometry.h"
#include "Bang/IEventsDestroy.h"
#include "Bang/List.h"
#include "Bang/List.tcc"
#include "Bang/Math.h"
#include "Bang/Matrix4.h"
#include "Bang/Matrix4.tcc"
#include "Bang/MetaNode.h"
#include "Bang/MetaNode.tcc"
#include "Bang/Quad.h"
#include "Bang/Scene.h"
#include "Bang/SceneManager.h"
#include "Bang/TextureCubeMap.h"
#include "Bang/TextureFactory.h"
#include "Bang/Transform.h"
#include "Bang/USet.tcc"
#include "Bang/Vector2.h"
#include "Bang/Vector3.h"
#include "Bang/Vector4.h"

namespace Bang
{
class ICloneable;
template <class>
class EventEmitter;
}  // namespace Bang

using namespace Bang;

Camera::Camera()
{
    SET_INSTANCE_CLASS_ID(Camera)

    AddRenderPass(RenderPass::SCENE_OPAQUE);
    AddRenderPass(RenderPass::SCENE_TRANSPARENT);
    AddRenderPass(RenderPass::SCENE_DECALS);
    AddRenderPass(RenderPass::SCENE_BEFORE_ADDING_LIGHTS);
    AddRenderPass(RenderPass::SCENE_AFTER_ADDING_LIGHTS);
    AddRenderPass(RenderPass::CANVAS);
    AddRenderPass(RenderPass::CANVAS_POSTPROCESS);
    AddRenderPass(RenderPass::OVERLAY);
    AddRenderPass(RenderPass::OVERLAY_POSTPROCESS);

    m_gbuffer = new GBuffer(1, 1);

    SetSkyBoxTexture(TextureFactory::GetDefaultSkybox());
    SetHDR(true);
}

Camera::~Camera()
{
    delete m_gbuffer;
}

void Camera::Bind() const
{
    GL::Push(GL::Pushable::VIEWPORT);
    GL::Push(GL::Pushable::VIEW_MATRIX);
    GL::Push(GL::Pushable::PROJECTION_MATRIX);

    if (Transform *tr = GetGameObject()->GetTransform())
    {
        GLUniforms::SetCameraWorldPosition(tr->GetPosition());
    }
    GLUniforms::SetCameraClearColor(GetClearColor());
    GLUniforms::SetCameraClearMode(GetClearMode());
    GLUniforms::SetViewMatrix(GetViewMatrix());
    GLUniforms::SetProjectionMatrix(GetProjectionMatrix());

    GL::SetViewport(0, 0, GetRenderSize().x, GetRenderSize().y);
    GetGBuffer()->Bind();
}

void Camera::UnBind() const
{
    GetGBuffer()->UnBind();
    GL::Pop(GL::Pushable::PROJECTION_MATRIX);
    GL::Pop(GL::Pushable::VIEW_MATRIX);
    GL::Pop(GL::Pushable::VIEWPORT);
}

Ray Camera::FromViewportPointNDCToRay(const Vector2 &vpPointNDC) const
{
    Vector3 worldPoint = FromViewportPointNDCToWorldPoint(vpPointNDC, 1);

    Ray ray;
    ray.SetOrigin(GetGameObject()->GetTransform()->GetPosition());
    ray.SetDirection((worldPoint - ray.GetOrigin()).Normalized());
    return ray;
}

Vector3 Camera::FromWorldPointToViewportPointNDC(
    const Vector3 &worldPosition) const
{
    Vector4 v4 =
        GetProjectionMatrix() * GetViewMatrix() * Vector4(worldPosition, 1);
    v4 /= v4.w;
    return v4.xyz();
}

Vector3 Camera::FromViewportPointNDCToWorldPoint(
    const Vector3 &vpPositionNDC) const
{
    return FromViewportPointNDCToWorldPoint(vpPositionNDC.xy(),
                                            vpPositionNDC.z);
}

Vector3 Camera::FromViewportPointNDCToWorldPoint(const Vector2 &vpPositionNDC,
                                                 float zNDC) const
{
    // 1 is zNear, -1 is zFar
    float zWorld =
        (GetZFar() - GetZNear()) * (-zNDC * 0.5f + 0.5f) + GetZNear();

    // Pass coordinates to clip space, to invert them using projInversed
    Vector4 clipCoords = Vector4(vpPositionNDC, 1, 1) * zWorld;
    Vector4 res4 = GetProjectionMatrix().Inversed() * clipCoords;
    Vector3 res = res4.xyz();
    res = (GetViewMatrix().Inversed() * Vector4(res, 1)).xyz();
    return res;
}

void Camera::SetReplacementGBuffer(GBuffer *gbuffer)
{
    p_replacementGBuffer = gbuffer;
}

void Camera::SetRenderFlags(RenderFlags renderFlags)
{
    if (renderFlags != GetRenderFlags())
    {
        m_renderFlags = renderFlags;
    }
}

void Camera::SetRenderSize(const Vector2i &renderSize)
{
    if (GetGBuffer()->Resize(renderSize))
    {
        GL::Push(GL::Pushable::FRAMEBUFFER_AND_READ_DRAW_ATTACHMENTS);
        GetGBuffer()->Bind();
        GetGBuffer()->SetAllDrawBuffers();
        GL::ClearColorStencilDepthBuffers();
        GL::Pop(GL::Pushable::FRAMEBUFFER_AND_READ_DRAW_ATTACHMENTS);
    }
}

void Camera::SetGammaCorrection(float gammaCorrection)
{
    if (gammaCorrection != GetGammaCorrection())
    {
        m_gammaCorrection = gammaCorrection;
    }
}

AARect Camera::GetViewportBoundingAARectNDC(const AABox &aaBBoxWorld) const
{
    Transform *tr = GetGameObject()->GetTransform();
    Vector3 camPosition = tr->GetPosition();
    if (aaBBoxWorld.Contains(camPosition))
    {
        return AARect::NDCRect();
    }

    Array<Vector3> intPoints;
    intPoints.PushBack(
        Geometry::IntersectQuadAABox(GetFrustumTopQuad(), aaBBoxWorld));
    intPoints.PushBack(
        Geometry::IntersectQuadAABox(GetFrustumBotQuad(), aaBBoxWorld));
    intPoints.PushBack(
        Geometry::IntersectQuadAABox(GetFrustumLeftQuad(), aaBBoxWorld));
    intPoints.PushBack(
        Geometry::IntersectQuadAABox(GetFrustumRightQuad(), aaBBoxWorld));
    intPoints.PushBack(
        Geometry::IntersectQuadAABox(GetFrustumNearQuad(), aaBBoxWorld));
    intPoints.PushBack(
        Geometry::IntersectQuadAABox(GetFrustumFarQuad(), aaBBoxWorld));

    Array<Vector3> boxPoints = aaBBoxWorld.GetPoints();
    for (const Vector3 &bp : boxPoints)
    {
        if (IsPointInsideFrustum(bp))
        {
            intPoints.PushBack(bp);
        }
    }

    List<Vector2> viewportPoints;
    for (const Vector3 &p : intPoints)
    {
        Vector2 viewportPoint = FromWorldPointToViewportPointNDC(p).xy();
        viewportPoints.PushBack(viewportPoint);
    }

    AARect boundingRect = AARect::GetBoundingRectFromPositions(
        viewportPoints.Begin(), viewportPoints.End());
    return boundingRect;
}

void Camera::SetOrthoHeight(float orthoHeight)
{
    m_orthoHeight = orthoHeight;
}

void Camera::SetClearColor(const Color &color)
{
    m_clearColor = color;
}

void Camera::SetFovDegrees(float fovDegrees)
{
    m_fovDegrees = fovDegrees;
}

void Camera::SetZNear(float zNear)
{
    m_zNear = zNear;
}

void Camera::SetZFar(float zFar)
{
    m_zFar = zFar;
}

void Camera::SetProjectionMode(CameraProjectionMode projMode)
{
    m_projMode = projMode;
}

void Camera::AddRenderPass(RenderPass renderPass)
{
    m_renderPassMask.Add(renderPass);
}

void Camera::RemoveRenderPass(RenderPass renderPass)
{
    m_renderPassMask.Remove(renderPass);
}

void Camera::SetSkyBoxTexture(TextureCubeMap *skyBoxTextureCM,
                              bool createFilteredCubeMapsForIBL)
{
    if (GetSkyBoxTexture() != skyBoxTextureCM)
    {
        p_skyboxTextureCM.Set(skyBoxTextureCM);

        if (createFilteredCubeMapsForIBL)
        {
            // If new, generate the IBL specular and diffuse textures!
            AH<TextureCubeMap> diffuseIBLCubeMap =
                CubeMapIBLGenerator::GenerateDiffuseIBLCubeMap(skyBoxTextureCM);
            p_skyboxDiffuseTextureCM.Set(diffuseIBLCubeMap.Get());

            AH<TextureCubeMap> specularIBLCubeMap =
                CubeMapIBLGenerator::GenerateSpecularIBLCubeMap(
                    skyBoxTextureCM);
            p_skyboxSpecularTextureCM.Set(specularIBLCubeMap.Get());
        }
        else
        {
            p_skyboxDiffuseTextureCM.Set(skyBoxTextureCM);
            p_skyboxSpecularTextureCM.Set(skyBoxTextureCM);
        }
    }
}

void Camera::SetClearMode(CameraClearMode clearMode)
{
    m_clearMode = clearMode;
}

void Camera::SetHDR(bool hdr)
{
    GetGBuffer()->SetHDR(hdr);
}

bool Camera::GetHDR() const
{
    return GetGBuffer()->GetHDR();
}

const Color &Camera::GetClearColor() const
{
    return m_clearColor;
}

float Camera::GetAspectRatio() const
{
    return SCAST<float>(GetRenderSize().x) / Math::Max(GetRenderSize().y, 1);
}

float Camera::GetOrthoHeight() const
{
    return m_orthoHeight;
}

float Camera::GetFovDegrees() const
{
    return m_fovDegrees;
}

float Camera::GetZNear() const
{
    return m_zNear;
}

float Camera::GetZFar() const
{
    return m_zFar;
}

CameraClearMode Camera::GetClearMode() const
{
    return m_clearMode;
}

float Camera::GetGammaCorrection() const
{
    return m_gammaCorrection;
}

RenderFlags Camera::GetRenderFlags() const
{
    return m_renderFlags;
}

bool Camera::MustRenderPass(RenderPass renderPass) const
{
    return GetRenderPassMask().Contains(renderPass);
}

const USet<RenderPass, EnumClassHash> &Camera::GetRenderPassMask() const
{
    return m_renderPassMask;
}

GBuffer *Camera::GetGBuffer() const
{
    return p_replacementGBuffer ? p_replacementGBuffer : m_gbuffer;
}

const Vector2i &Camera::GetRenderSize() const
{
    return GetGBuffer()->GetSize();
}

TextureCubeMap *Camera::GetSkyBoxTexture() const
{
    return p_skyboxTextureCM.Get();
}

TextureCubeMap *Camera::GetSpecularSkyBoxTexture() const
{
    return p_skyboxSpecularTextureCM.Get();
}

TextureCubeMap *Camera::GetDiffuseSkyBoxTexture() const
{
    return p_skyboxDiffuseTextureCM.Get();
}

Quad Camera::GetFrustumNearQuad() const
{
    Vector3 p0 = FromViewportPointNDCToWorldPoint(Vector3(-1, -1, 1));
    Vector3 p1 = FromViewportPointNDCToWorldPoint(Vector3(1, -1, 1));
    Vector3 p2 = FromViewportPointNDCToWorldPoint(Vector3(1, 1, 1));
    Vector3 p3 = FromViewportPointNDCToWorldPoint(Vector3(-1, 1, 1));
    return Quad(p0, p1, p2, p3);
}
Quad Camera::GetFrustumFarQuad() const
{
    Vector3 p0 = FromViewportPointNDCToWorldPoint(Vector3(1, -1, -1));
    Vector3 p1 = FromViewportPointNDCToWorldPoint(Vector3(-1, -1, -1));
    Vector3 p2 = FromViewportPointNDCToWorldPoint(Vector3(-1, 1, -1));
    Vector3 p3 = FromViewportPointNDCToWorldPoint(Vector3(1, 1, -1));
    return Quad(p0, p1, p2, p3);
}
Quad Camera::GetFrustumLeftQuad() const
{
    Vector3 p0 = FromViewportPointNDCToWorldPoint(Vector3(-1, -1, -1));
    Vector3 p1 = FromViewportPointNDCToWorldPoint(Vector3(-1, -1, 1));
    Vector3 p2 = FromViewportPointNDCToWorldPoint(Vector3(-1, 1, 1));
    Vector3 p3 = FromViewportPointNDCToWorldPoint(Vector3(-1, 1, -1));
    return Quad(p0, p1, p2, p3);
}
Quad Camera::GetFrustumRightQuad() const
{
    Vector3 p0 = FromViewportPointNDCToWorldPoint(Vector3(1, -1, 1));
    Vector3 p1 = FromViewportPointNDCToWorldPoint(Vector3(1, -1, -1));
    Vector3 p2 = FromViewportPointNDCToWorldPoint(Vector3(1, 1, -1));
    Vector3 p3 = FromViewportPointNDCToWorldPoint(Vector3(1, 1, 1));
    return Quad(p0, p1, p2, p3);
}
Quad Camera::GetFrustumTopQuad() const
{
    Vector3 p0 = FromViewportPointNDCToWorldPoint(Vector3(-1, 1, 1));
    Vector3 p1 = FromViewportPointNDCToWorldPoint(Vector3(1, 1, 1));
    Vector3 p2 = FromViewportPointNDCToWorldPoint(Vector3(1, 1, -1));
    Vector3 p3 = FromViewportPointNDCToWorldPoint(Vector3(-1, 1, -1));
    return Quad(p0, p1, p2, p3);
}
Quad Camera::GetFrustumBotQuad() const
{
    Vector3 p0 = FromViewportPointNDCToWorldPoint(Vector3(-1, -1, 1));
    Vector3 p1 = FromViewportPointNDCToWorldPoint(Vector3(-1, -1, -1));
    Vector3 p2 = FromViewportPointNDCToWorldPoint(Vector3(1, -1, -1));
    Vector3 p3 = FromViewportPointNDCToWorldPoint(Vector3(1, -1, 1));
    return Quad(p0, p1, p2, p3);
}

Camera *Camera::GetActive()
{
    Camera *cam = GEngine::GetActiveRenderingCamera();
    if (!cam)
    {
        Scene *activeScene = SceneManager::GetActiveScene();
        cam = activeScene ? activeScene->GetCamera() : nullptr;
    }
    return cam;
}

CameraProjectionMode Camera::GetProjectionMode() const
{
    return m_projMode;
}

float Camera::GetOrthoWidth() const
{
    return GetOrthoHeight() * GetAspectRatio();
}

Matrix4 Camera::GetViewMatrix() const
{
    Transform *tr = GetGameObject()->GetTransform();
    Matrix4 localToWorld = Matrix4::TranslateMatrix(tr->GetPosition()) *
                           Matrix4::RotateMatrix(tr->GetRotation());
    return localToWorld.Inversed();
}

Matrix4 Camera::GetProjectionMatrix() const
{
    if (m_projMode == CameraProjectionMode::PERSPECTIVE)
    {
        if (GetAspectRatio() == 0.0 || GetFovDegrees() == 0.0 ||
            GetZNear() == GetZFar())
        {
            return Matrix4::Identity();
        }

        return Matrix4::Perspective(Math::DegToRad(GetFovDegrees()),
                                    GetAspectRatio(),
                                    GetZNear(),
                                    GetZFar());
    }
    else  // Ortho
    {
        return Matrix4::Ortho(-GetOrthoWidth(),
                              GetOrthoWidth(),
                              -GetOrthoHeight(),
                              GetOrthoHeight(),
                              GetZNear(),
                              GetZFar());
    }
}

bool Camera::IsPointInsideFrustum(const Vector3 &worldPoint) const
{
    Vector3 projPoint = FromWorldPointToViewportPointNDC(worldPoint);
    return projPoint.x > -1.0f && projPoint.x < 1.0f && projPoint.y > -1.0f &&
           projPoint.y < 1.0f && projPoint.z > -1.0f && projPoint.z < 1.0f;
}

void Camera::CloneInto(ICloneable *clone, bool cloneGUID) const
{
    Component::CloneInto(clone, cloneGUID);
    Camera *cam = SCAST<Camera *>(clone);
    cam->SetZFar(GetZFar());
    cam->SetZNear(GetZNear());
    cam->SetFovDegrees(GetFovDegrees());
    cam->SetOrthoHeight(GetOrthoHeight());
    cam->SetProjectionMode(GetProjectionMode());
    cam->SetClearMode(GetClearMode());
    cam->SetClearColor(GetClearColor());
    cam->SetSkyBoxTexture(GetSkyBoxTexture());
}

void Camera::ImportMeta(const MetaNode &meta)
{
    Component::ImportMeta(meta);

    if (meta.Contains("FOVDegrees"))
    {
        SetFovDegrees(meta.Get<float>("FOVDegrees"));
    }

    if (meta.Contains("ZNear"))
    {
        SetZNear(meta.Get<float>("ZNear"));
    }

    if (meta.Contains("ZFar"))
    {
        SetZFar(meta.Get<float>("ZFar"));
    }

    if (meta.Contains("OrthoHeight"))
    {
        SetOrthoHeight(meta.Get<float>("OrthoHeight"));
    }

    if (meta.Contains("ProjectionMode"))
    {
        SetProjectionMode(meta.Get<CameraProjectionMode>("ProjectionMode"));
    }

    if (meta.Contains("ClearMode"))
    {
        SetClearMode(meta.Get<CameraClearMode>("ClearMode"));
    }

    if (meta.Contains("ClearColor"))
    {
        SetClearColor(meta.Get<Color>("ClearColor"));
    }

    if (meta.Contains("SkyBoxTexture"))
    {
        AH<TextureCubeMap> skyCM =
            Assets::Load<TextureCubeMap>(meta.Get<GUID>("SkyBoxTexture"));
        SetSkyBoxTexture(skyCM.Get());
    }
}

void Camera::ExportMeta(MetaNode *metaNode) const
{
    Component::ExportMeta(metaNode);

    metaNode->Set("ZNear", GetZNear());
    metaNode->Set("ZFar", GetZFar());
    metaNode->Set("ProjectionMode", GetProjectionMode());
    metaNode->Set("OrthoHeight", GetOrthoHeight());
    metaNode->Set("FOVDegrees", GetFovDegrees());
    metaNode->Set("ClearMode", GetClearMode());
    metaNode->Set("ClearColor", GetClearColor());
    metaNode->Set(
        "SkyBoxTexture",
        (GetSkyBoxTexture() ? GetSkyBoxTexture()->GetGUID() : GUID::Empty()));
}

void Camera::OnDestroyed(EventEmitter<IEventsDestroy> *object)
{
}
