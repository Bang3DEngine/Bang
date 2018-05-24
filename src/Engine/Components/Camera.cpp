#include "Bang/Camera.h"

#include "Bang/GL.h"
#include "Bang/Math.h"
#include "Bang/Mesh.h"
#include "Bang/AABox.h"
#include "Bang/Scene.h"
#include "Bang/AARect.h"
#include "Bang/Gizmos.h"
#include "Bang/Window.h"
#include "Bang/GBuffer.h"
#include "Bang/GEngine.h"
#include "Bang/Matrix4.h"
#include "Bang/Vector2.h"
#include "Bang/Vector3.h"
#include "Bang/Geometry.h"
#include "Bang/Resources.h"
#include "Bang/Transform.h"
#include "Bang/Texture2D.h"
#include "Bang/GameObject.h"
#include "Bang/GLUniforms.h"
#include "Bang/TextureFactory.h"
#include "Bang/MeshFactory.h"
#include "Bang/SceneManager.h"
#include "Bang/ShaderProgram.h"
#include "Bang/TextureCubeMap.h"
#include "Bang/CubeMapIBLGenerator.h"
#include "Bang/SelectionFramebuffer.h"

USING_NAMESPACE_BANG

Camera::Camera()
{
    AddRenderPass(RenderPass::SCENE);
    AddRenderPass(RenderPass::SCENE_POSTPROCESS);
    AddRenderPass(RenderPass::CANVAS);
    AddRenderPass(RenderPass::CANVAS_POSTPROCESS);

    m_gbuffer = new GBuffer(1,1);
    m_selectionFramebuffer = new SelectionFramebuffer(1,1);

    SetSkyBoxTexture(nullptr);
}

Camera::~Camera()
{
    delete m_gbuffer;
    delete m_selectionFramebuffer;
}

void Camera::Bind() const
{
    GLUniforms::SetViewMatrix( GetViewMatrix() );
    GLUniforms::SetProjectionMatrix( GetProjectionMatrix() );
    BindViewportForRendering();
}

void Camera::UnBind() const
{
    GetGBuffer()->UnBind();
    GetSelectionFramebuffer()->UnBind();
}

void Camera::BindViewportForBlitting() const
{
    GL::SetViewport( AARecti( GetViewportAARectInWindow() ) );
}

void Camera::BindViewportForRendering() const
{
    BindViewportForBlitting();
    AARecti vpRect = GL::GetViewportRect();
    GL::SetViewport(0, 0, vpRect.GetWidth(), vpRect.GetHeight());
}

void Camera::BindGBuffer()
{
    Vector2i vpSize = GL::GetViewportSize();
    GetGBuffer()->Resize(vpSize.x, vpSize.y);
    GetGBuffer()->Bind();
}

void Camera::BindSelectionFramebuffer()
{
    Vector2i vpSize = GL::GetViewportSize();
    GetSelectionFramebuffer()->Resize(vpSize.x, vpSize.y);

    GetSelectionFramebuffer()->Bind();
    GL::ClearStencilBuffer();
    GL::ClearDepthBuffer();
    GL::ClearColorBuffer(Color::Zero);
}

Ray Camera::FromViewportPointNDCToRay(const Vector2 &vpPointNDC) const
{
    Vector3 worldPoint = FromViewportPointNDCToWorldPoint(vpPointNDC, 1);

    Ray ray;
    ray.SetOrigin( GetGameObject()->GetTransform()->GetPosition() );
    ray.SetDirection( (worldPoint - ray.GetOrigin()).Normalized() );
    return ray;
}

Vector2i Camera::FromWindowPointToViewportPoint(const Vector2i &winPoint) const
{
    return Vector2i(
                GL::FromWindowPointToViewportPoint(Vector2(winPoint),
                                 AARecti(GetViewportAARectInWindow())) );
}

Vector3 Camera::FromWorldPointToViewportPointNDC(const Vector3 &worldPosition) const
{
    Vector4 v4 = GetProjectionMatrix() *
                 GetViewMatrix() * Vector4(worldPosition, 1);
    v4 /= v4.w;
    return v4.xyz();
}

Vector3 Camera::FromViewportPointNDCToWorldPoint(const Vector3 &vpPositionNDC) const
{
    return FromViewportPointNDCToWorldPoint(vpPositionNDC.xy(), vpPositionNDC.z);
}

Vector3 Camera::FromViewportPointNDCToWorldPoint(const Vector2 &vpPositionNDC,
                                                 float zNDC) const
{
    // 1 is zNear, -1 is zFar
    float zWorld = (GetZFar() - GetZNear()) * (-zNDC * 0.5f + 0.5f) +
                    GetZNear();

    // Pass coordinates to clip space, to invert them using projInversed
    Vector4 clipCoords = Vector4(vpPositionNDC, 1, 1) * zWorld;
    Vector4 res4 = GetProjectionMatrix().Inversed() * clipCoords;
    Vector3 res = res4.xyz();
    res = (GetViewMatrix().Inversed() * Vector4(res, 1)).xyz();
    return res;
}

AARect Camera::GetViewportBoundingAARectNDC(const AABox &aaBBoxWorld) const
{
    Transform *tr = GetGameObject()->GetTransform();
    Vector3 camPosition = tr->GetPosition();
    if ( aaBBoxWorld.Contains(camPosition) ) { return AARect::NDCRect; }

    Array<Vector3> intPoints;
    intPoints.PushBack(Geometry::IntersectQuadAABox(GetFrustumTopQuad(),   aaBBoxWorld));
    intPoints.PushBack(Geometry::IntersectQuadAABox(GetFrustumBotQuad(),   aaBBoxWorld));
    intPoints.PushBack(Geometry::IntersectQuadAABox(GetFrustumLeftQuad(),  aaBBoxWorld));
    intPoints.PushBack(Geometry::IntersectQuadAABox(GetFrustumRightQuad(), aaBBoxWorld));
    intPoints.PushBack(Geometry::IntersectQuadAABox(GetFrustumNearQuad(),  aaBBoxWorld));
    intPoints.PushBack(Geometry::IntersectQuadAABox(GetFrustumFarQuad(),   aaBBoxWorld));

    Array<Vector3> boxPoints = aaBBoxWorld.GetPoints();
    for (const Vector3 &bp : boxPoints)
    {
        if (IsPointInsideFrustum(bp)) { intPoints.PushBack(bp); }
    }


    List<Vector2> viewportPoints;
    for (const Vector3 &p : intPoints)
    {
        Vector2 viewportPoint = FromWorldPointToViewportPointNDC(p).xy();
        viewportPoints.PushBack(viewportPoint);
    }

    AARect boundingRect = AARect::GetBoundingRectFromPositions(viewportPoints.Begin(),
                                                               viewportPoints.End());
    return boundingRect;
}

void Camera::SetOrthoHeight(float orthoHeight) { m_orthoHeight = orthoHeight; }
void Camera::SetClearColor(const Color &color) { m_clearColor = color; }
void Camera::SetFovDegrees(float fovDegrees) { this->m_fovDegrees = fovDegrees; }
void Camera::SetZNear(float zNear) { this->m_zNear = zNear; }
void Camera::SetZFar(float zFar) { this->m_zFar = zFar; }

void Camera::SetProjectionMode(Camera::ProjectionMode projMode)
{
    m_projMode = projMode;
}

void Camera::SetViewportAARectNDC(const AARect &viewportRectNDC)
{
    m_viewportRectNDC = viewportRectNDC;
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
            RH<TextureCubeMap> diffuseIBLCubeMap = CubeMapIBLGenerator::
                                   GenerateDiffuseIBLCubeMap(skyBoxTextureCM);
            p_skyboxDiffuseTextureCM.Set( diffuseIBLCubeMap.Get() );

            RH<TextureCubeMap> specularIBLCubeMap = CubeMapIBLGenerator::
                                   GenerateSpecularIBLCubeMap(skyBoxTextureCM);
            p_skyboxSpecularTextureCM.Set( specularIBLCubeMap.Get() );
        }
        else
        {
            p_skyboxDiffuseTextureCM.Set( skyBoxTextureCM );
            p_skyboxSpecularTextureCM.Set( skyBoxTextureCM );
        }
    }
}

void Camera::SetRenderSelectionBuffer(bool renderSelectionBuffer)
{
    m_renderSelectionBuffer = renderSelectionBuffer;
}

void Camera::SetClearMode(Camera::ClearMode clearMode)
{
    m_clearMode = clearMode;
}

const Color &Camera::GetClearColor() const { return m_clearColor; }
float Camera::GetOrthoHeight() const { return m_orthoHeight; }
float Camera::GetFovDegrees() const { return m_fovDegrees; }
float Camera::GetZNear() const { return m_zNear; }
float Camera::GetZFar() const { return m_zFar; }

Camera::ClearMode Camera::GetClearMode() const
{
    return m_clearMode;
}

bool Camera::MustRenderPass(RenderPass renderPass) const
{
    return GetRenderPassMask().Contains(renderPass);
}

const Set<RenderPass> &Camera::GetRenderPassMask() const
{
    return m_renderPassMask;
}

AARect Camera::GetViewportAARectInWindow() const
{
    AARect vpRect = GetViewportAARectNDC() * 0.5f + 0.5f;
    return AARect( vpRect * Vector2(GL::GetViewportSize())
                   + Vector2(GL::GetViewportRect().GetMin()) );
}
AARect Camera::GetViewportAARectNDCInWindow() const
{
    return GL::FromWindowRectToWindowRectNDC( GetViewportAARectInWindow() );
}

const AARect& Camera::GetViewportAARectNDC() const
{
    return m_viewportRectNDC;
}

GBuffer *Camera::GetGBuffer() const
{
    return m_gbuffer;
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

SelectionFramebuffer *Camera::GetSelectionFramebuffer() const
{
    return m_selectionFramebuffer;
}

Quad Camera::GetFrustumNearQuad() const
{
    Vector3 p0 = FromViewportPointNDCToWorldPoint( Vector3(-1, -1,  1) );
    Vector3 p1 = FromViewportPointNDCToWorldPoint( Vector3( 1, -1,  1) );
    Vector3 p2 = FromViewportPointNDCToWorldPoint( Vector3( 1,  1,  1) );
    Vector3 p3 = FromViewportPointNDCToWorldPoint( Vector3(-1,  1,  1) );
    return Quad(p0, p1, p2, p3);
}
Quad Camera::GetFrustumFarQuad() const
{
    Vector3 p0 = FromViewportPointNDCToWorldPoint( Vector3( 1, -1, -1) );
    Vector3 p1 = FromViewportPointNDCToWorldPoint( Vector3(-1, -1, -1) );
    Vector3 p2 = FromViewportPointNDCToWorldPoint( Vector3(-1,  1, -1) );
    Vector3 p3 = FromViewportPointNDCToWorldPoint( Vector3( 1,  1, -1) );
    return Quad(p0, p1, p2, p3);
}
Quad Camera::GetFrustumLeftQuad() const
{
    Vector3 p0 = FromViewportPointNDCToWorldPoint( Vector3(-1, -1, -1) );
    Vector3 p1 = FromViewportPointNDCToWorldPoint( Vector3(-1, -1,  1) );
    Vector3 p2 = FromViewportPointNDCToWorldPoint( Vector3(-1,  1,  1) );
    Vector3 p3 = FromViewportPointNDCToWorldPoint( Vector3(-1,  1, -1) );
    return Quad(p0, p1, p2, p3);
}
Quad Camera::GetFrustumRightQuad() const
{
    Vector3 p0 = FromViewportPointNDCToWorldPoint( Vector3( 1, -1,  1) );
    Vector3 p1 = FromViewportPointNDCToWorldPoint( Vector3( 1, -1, -1) );
    Vector3 p2 = FromViewportPointNDCToWorldPoint( Vector3( 1,  1, -1) );
    Vector3 p3 = FromViewportPointNDCToWorldPoint( Vector3( 1,  1,  1) );
    return Quad(p0, p1, p2, p3);
}
Quad Camera::GetFrustumTopQuad() const
{
    Vector3 p0 = FromViewportPointNDCToWorldPoint( Vector3(-1,  1,  1) );
    Vector3 p1 = FromViewportPointNDCToWorldPoint( Vector3( 1,  1,  1) );
    Vector3 p2 = FromViewportPointNDCToWorldPoint( Vector3( 1,  1, -1) );
    Vector3 p3 = FromViewportPointNDCToWorldPoint( Vector3(-1,  1, -1) );
    return Quad(p0, p1, p2, p3);
}
Quad Camera::GetFrustumBotQuad() const
{
    Vector3 p0 = FromViewportPointNDCToWorldPoint( Vector3(-1, -1,  1) );
    Vector3 p1 = FromViewportPointNDCToWorldPoint( Vector3(-1, -1, -1) );
    Vector3 p2 = FromViewportPointNDCToWorldPoint( Vector3( 1, -1, -1) );
    Vector3 p3 = FromViewportPointNDCToWorldPoint( Vector3( 1, -1,  1) );
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
Camera::ProjectionMode Camera::GetProjectionMode() const { return m_projMode; }

float Camera::GetOrthoWidth() const
{
   return GetOrthoHeight() * GL::GetViewportAspectRatio();
}

Matrix4 Camera::GetViewMatrix() const
{
    Transform *tr = GetGameObject()->GetTransform();
    return tr->GetLocalToWorldMatrixInv();
}

bool Camera::GetRenderSelectionBuffer() const
{
    return m_renderSelectionBuffer;
}

Matrix4 Camera::GetProjectionMatrix() const
{
    if (m_projMode == ProjectionMode::PERSPECTIVE)
    {
        if (GL::GetViewportAspectRatio() == 0.0 ||
            GetFovDegrees() == 0.0 ||
            GetZNear() == GetZFar())
        {
            return Matrix4::Identity;
        }

        return Matrix4::Perspective(Math::DegToRad(GetFovDegrees()),
                                    GL::GetViewportAspectRatio(),
                                    GetZNear(), GetZFar());
    }
    // else // Ortho
    {
        return Matrix4::Ortho(-GetOrthoWidth(),  GetOrthoWidth(),
                              -GetOrthoHeight(), GetOrthoHeight(),
                               GetZNear(),       GetZFar());
    }
}

bool Camera::IsPointInsideFrustum(const Vector3 &worldPoint) const
{
    Vector3 projPoint = FromWorldPointToViewportPointNDC(worldPoint);
    return projPoint.x > -1.0f && projPoint.x < 1.0f &&
           projPoint.y > -1.0f && projPoint.y < 1.0f &&
           projPoint.z > -1.0f && projPoint.z < 1.0f;
}

void Camera::OnRender(RenderPass rp)
{
    Component::OnRender(rp);
    if (rp != RenderPass::OVERLAY) { return; }

    Gizmos::Reset();
    static RH<Mesh> cameraMesh = MeshFactory::GetCamera();
    Transform *camTransform = GetGameObject()->GetTransform();
    float distScale = 15.0f;

    /*
    Camera *sceneCam = SceneManager::GetActiveScene()->GetCamera();
    Transform *sceneCamTransform = sceneCam->GetGameObject()->GetTransform();
    float distScale = Vector3::Distance(sceneCamTransform->GetPosition(),
                                        camTransform->GetPosition());
    */

    Gizmos::SetReceivesLighting(true);
    Gizmos::SetSelectable(GetGameObject());
    Gizmos::SetPosition(camTransform->GetPosition());
    Gizmos::SetRotation(camTransform->GetRotation());
    Gizmos::SetScale(Vector3::One * 0.02f * distScale);
    Gizmos::SetColor(Color::White);
    Gizmos::RenderCustomMesh(cameraMesh.Get());
}

void Camera::CloneInto(ICloneable *clone) const
{
    Component::CloneInto(clone);
    Camera *cam = Cast<Camera*>(clone);
    cam->SetZFar(GetZFar());
    cam->SetZNear(GetZNear());
    cam->SetFovDegrees(GetFovDegrees());
    cam->SetOrthoHeight(GetOrthoHeight());
    cam->SetProjectionMode(GetProjectionMode());
    cam->SetClearMode(GetClearMode());
    cam->SetClearColor(GetClearColor());
    cam->SetSkyBoxTexture(GetSkyBoxTexture());
}

#include "Bang/Image.h"
void Camera::ImportXML(const XMLNode &xml)
{
    Component::ImportXML(xml);

    if (xml.Contains("FOVDegrees"))
    { SetFovDegrees(xml.Get<float>("FOVDegrees")); }

    if (xml.Contains("ZNear"))
    { SetZNear(xml.Get<float>("ZNear")); }

    if (xml.Contains("ZFar"))
    { SetZFar(xml.Get<float>("ZFar")); }

    if (xml.Contains("OrthoHeight"))
    { SetOrthoHeight( xml.Get<float>("OrthoHeight") ); }

    if (xml.Contains("ProjectionMode"))
    { SetProjectionMode( xml.Get<ProjectionMode>("ProjectionMode") ); }

    if (xml.Contains("ClearMode"))
    { SetClearMode( xml.Get<ClearMode>("ClearMode") ); }

    if (xml.Contains("ClearColor"))
    { SetClearColor(xml.Get<Color>("ClearColor")); }

    if (xml.Contains("SkyBoxTexture"))
    {
        RH<TextureCubeMap> skyCM = Resources::Load<TextureCubeMap>(
                                            xml.Get<GUID>("SkyBoxTexture") );
        SetSkyBoxTexture( skyCM.Get() );
    }
}

void Camera::ExportXML(XMLNode *xmlInfo) const
{
    Component::ExportXML(xmlInfo);

    xmlInfo->Set("ZNear", GetZNear());
    xmlInfo->Set("ZFar", GetZFar());
    xmlInfo->Set("ProjectionMode", GetProjectionMode());
    xmlInfo->Set("OrthoHeight", GetOrthoHeight());
    xmlInfo->Set("FOVDegrees", GetFovDegrees());
    xmlInfo->Set("ClearMode", GetClearMode());
    xmlInfo->Set("ClearColor", GetClearColor());
    xmlInfo->Set("SkyBoxTexture", (GetSkyBoxTexture() ?
                                     GetSkyBoxTexture()->GetGUID() :
                                     GUID::Empty()) );
}

void Camera::OnDestroyed(EventEmitter<IDestroyListener> *object)
{
}
