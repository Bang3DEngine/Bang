#include "Bang/DirectionalLight.h"

#include <GL/glew.h>
#include <array>
#include <vector>

#include "Bang/AABox.h"
#include "Bang/Array.tcc"
#include "Bang/Assert.h"
#include "Bang/Assets.h"
#include "Bang/Camera.h"
#include "Bang/ClassDB.h"
#include "Bang/Framebuffer.h"
#include "Bang/GEngine.h"
#include "Bang/GL.h"
#include "Bang/GLUniforms.h"
#include "Bang/GameObject.h"
#include "Bang/Geometry.h"
#include "Bang/Math.h"
#include "Bang/Matrix4.h"
#include "Bang/Matrix4.tcc"
#include "Bang/MetaNode.h"
#include "Bang/MetaNode.tcc"
#include "Bang/Quad.h"
#include "Bang/RenderPass.h"
#include "Bang/Renderer.h"
#include "Bang/ShaderProgram.h"
#include "Bang/ShaderProgramFactory.h"
#include "Bang/Texture2D.h"
#include "Bang/Transform.h"
#include "Bang/Vector3.h"
#include "Bang/Vector4.h"

namespace Bang
{
class ICloneable;
}

using namespace Bang;

#include "Bang/Input.h"
DirectionalLight::DirectionalLight()
{
    SET_INSTANCE_CLASS_ID(DirectionalLight)

    m_blurAuxiliarTexture = Assets::Create<Texture2D>();
    m_blurAuxiliarTexture.Get()->SetFormat(GL::ColorFormat::RGBA32F);
    m_blurAuxiliarTexture.Get()->SetFilterMode(GL::FilterMode::BILINEAR);
    m_blurAuxiliarTexture.Get()->SetWrapMode(GL::WrapMode::CLAMP_TO_EDGE);

    m_blurredShadowMapTexture = Assets::Create<Texture2D>();
    m_blurredShadowMapTexture.Get()->SetFormat(GL::ColorFormat::RGBA32F);
    m_blurredShadowMapTexture.Get()->SetFilterMode(GL::FilterMode::BILINEAR);
    m_blurredShadowMapTexture.Get()->SetWrapMode(GL::WrapMode::CLAMP_TO_EDGE);

    m_shadowMapFramebuffer = new Framebuffer();
    m_shadowMapFramebuffer->CreateAttachmentTex2D(GL::Attachment::COLOR0,
                                                  GL::ColorFormat::RGBA32F);
    m_shadowMapFramebuffer->GetAttachmentTex2D(GL::Attachment::COLOR0)
        ->SetWrapMode(GL::WrapMode::REPEAT);
    m_shadowMapFramebuffer->GetAttachmentTex2D(GL::Attachment::COLOR0)
        ->SetFilterMode(GL::FilterMode::BILINEAR);
    m_shadowMapFramebuffer->CreateAttachmentTex2D(GL::Attachment::DEPTH,
                                                  GL::ColorFormat::DEPTH16);

    SetShadowMapShaderProgram(
        ShaderProgramFactory::GetDirectionalLightShadowMap());
    SetLightScreenPassShaderProgram(
        ShaderProgramFactory::GetDirectionalLightDeferredScreenPass());
}

DirectionalLight::~DirectionalLight()
{
    delete m_shadowMapFramebuffer;
}

AABox DirectionalLight::GetShadowCastersAABox(
    const Array<Renderer *> &shadowCastersRenderers) const
{
    Array<Vector3> casterPoints;
    for (Renderer *shadowCasterRend : shadowCastersRenderers)
    {
        Matrix4 localToWorld = shadowCasterRend->GetGameObject()
                                   ->GetTransform()
                                   ->GetLocalToWorldMatrix();
        AABox rendAABox = shadowCasterRend->GetAABBox();
        if (rendAABox != AABox::Empty())
        {
            AABox rendAABoxWorld = localToWorld * rendAABox;
            casterPoints.PushBack(rendAABoxWorld.GetPoints());
        }
    }

    AABox sceneAABox;
    sceneAABox.CreateFromPositions(casterPoints);
    return sceneAABox;
}

void DirectionalLight::RenderShadowMaps_(GameObject *go)
{
    GL::Push(GL::Pushable::VIEWPORT);
    GL::Push(GL::Pushable::COLOR_MASK);
    GL::Push(GL::Pushable::DEPTH_STATES);
    GL::Push(GL::Pushable::ALL_MATRICES);
    GL::Push(GL::Pushable::FRAMEBUFFER_AND_READ_DRAW_ATTACHMENTS);

    GEngine *ge = GEngine::GetInstance();
    ge->PushActiveRenderingCamera();

    // Bind and resize shadow map framebuffer
    const Vector2i &shadowMapSize = GetShadowMapSize();
    m_shadowMapFramebuffer->Bind();
    m_shadowMapFramebuffer->Resize(shadowMapSize);
    m_shadowMapFramebuffer->SetAllDrawBuffers();

    // Set up viewport
    GL::SetViewport(0, 0, shadowMapSize.x, shadowMapSize.y);

    // Set up shadow map matrices
    const Array<Renderer *> shadowCastersRenderers = GetShadowCastersIn(go);
    Matrix4 shadowMapViewMatrix, shadowMapProjMatrix;
    GetWorldToShadowMapMatrices(
        &shadowMapViewMatrix, &shadowMapProjMatrix, shadowCastersRenderers);
    GLUniforms::SetModelMatrix(Matrix4::Identity());
    GLUniforms::SetViewMatrix(shadowMapViewMatrix);
    GLUniforms::SetProjectionMatrix(shadowMapProjMatrix);
    m_lastUsedShadowMapViewProj = shadowMapProjMatrix * shadowMapViewMatrix;

    // Render shadow map into framebuffer
    GL::SetDepthMask(true);
    GL::ClearDepthBuffer(1.0f);
    GL::SetDepthFunc(GL::Function::LEQUAL);
    float limit = Math::Exp(GetShadowExponentConstant());
    GL::ClearColorBuffer(Color(limit));

    for (Renderer *rend : shadowCastersRenderers)
    {
        rend->OnRender(RenderPass::SCENE_OPAQUE);
    }

    ge->PopActiveRenderingCamera();

    // Blur shadow map
    if (GetShadowSoftness() > 0)
    {
        GEngine::GetInstance()->BlurTexture(
            m_shadowMapFramebuffer->GetAttachmentTex2D(GL::Attachment::COLOR0),
            m_blurAuxiliarTexture.Get(),
            m_blurredShadowMapTexture.Get(),
            GetShadowSoftness(),
            BlurType::KAWASE);
    }

    GL::Pop(GL::Pushable::FRAMEBUFFER_AND_READ_DRAW_ATTACHMENTS);
    GL::Pop(GL::Pushable::ALL_MATRICES);
    GL::Pop(GL::Pushable::DEPTH_STATES);
    GL::Pop(GL::Pushable::COLOR_MASK);
    GL::Pop(GL::Pushable::VIEWPORT);
}

void DirectionalLight::SetUniformsBeforeApplyingLight(ShaderProgram *sp) const
{
    Light::SetUniformsBeforeApplyingLight(sp);

    ASSERT(GL::IsBound(sp))
    sp->SetMatrix4(
        "B_LightWorldToShadowMapMatrix", m_lastUsedShadowMapViewProj, false);
}

void DirectionalLight::Reflect()
{
    Light::Reflect();

    BANG_REFLECT_VAR_MEMBER_HINTED(DirectionalLight,
                                   "Shadow Distance",
                                   SetShadowDistance,
                                   GetShadowDistance,
                                   BANG_REFLECT_HINT_MIN_VALUE(0.0f));
}

void DirectionalLight::SetShadowDistance(float shadowDistance)
{
    m_shadowDistance = shadowDistance;
}

float DirectionalLight::GetShadowMapNearDistance() const
{
    return 0.05f;
}

float DirectionalLight::GetShadowMapFarDistance() const
{
    return GetShadowDistance();
}

float DirectionalLight::GetShadowDistance() const
{
    return m_shadowDistance;
}

Texture2D *DirectionalLight::GetShadowMapTexture() const
{
    return GetShadowSoftness() > 0 ? m_blurredShadowMapTexture.Get()
                                   : m_shadowMapFramebuffer->GetAttachmentTex2D(
                                         GL::Attachment::COLOR0);
}

void DirectionalLight::GetWorldToShadowMapMatrices(
    Matrix4 *viewMatrix,
    Matrix4 *projMatrix,
    const Array<Renderer *> &shadowCastersRenderers) const
{
    // The ortho box will be the AABox in light space of the AABox of the
    // scene in world space
    AABox orthoBoxInLightSpace = GetShadowMapOrthoBox(shadowCastersRenderers);
    Vector3 orthoBoxExtents = orthoBoxInLightSpace.GetExtents();
    Matrix4 lightToWorld = GetLightToWorldMatrix();
    Vector3 fwd = lightToWorld.TransformedVector(Vector3::Forward());
    Vector3 up = lightToWorld.TransformedVector(Vector3::Up());

    Vector3 orthoBoxCenterWorld =
        lightToWorld.TransformedPoint(orthoBoxInLightSpace.GetCenter());

    *viewMatrix =
        Matrix4::LookAt(orthoBoxCenterWorld, orthoBoxCenterWorld + fwd, up);

    *projMatrix = Matrix4::Ortho(-orthoBoxExtents.x,
                                 orthoBoxExtents.x,
                                 -orthoBoxExtents.y,
                                 orthoBoxExtents.y,
                                 -orthoBoxExtents.z,
                                 orthoBoxExtents.z);
}

AABox DirectionalLight::GetShadowMapOrthoBox(
    const Array<Renderer *> &shadowCastersRenderers) const
{
    // Adjust zFar so that we take into account shadow distance, and get our
    // camera frustum quads, then restore zFar
    Camera *cam = Camera::GetActive();  // Get active camera
    float prevZFar = cam->GetZFar();    // Save for later restore
    cam->SetZFar(Math::Min(prevZFar, GetShadowDistance()));  // Shadow distance
    const Quad camTopQuad = cam->GetFrustumTopQuad();        // Get top quad
    const Quad camBotQuad = cam->GetFrustumBotQuad();        // Get bot quad
    const Quad camLeftQuad = cam->GetFrustumLeftQuad();      // Get left quad
    const Quad camRightQuad = cam->GetFrustumRightQuad();    // Get right quad
    const std::array<Quad, 4> camQuads = {
        {camTopQuad, camBotQuad, camLeftQuad, camRightQuad}};
    cam->SetZFar(prevZFar);  // Restore

    // Get all camera frustum points in world space
    Array<Vector3> camFrustumPointsWS;
    for (const Quad &quad : camQuads)
    {
        for (const Vector3 &p : quad.GetPoints())
        {
            camFrustumPointsWS.PushBack(p);
        }
    }

    // Get light space matrix
    const Matrix4 lightToWorld = GetLightToWorldMatrix();
    const Matrix4 worldToLight = lightToWorld.Inversed();

    // Get the cam frustum AABox in light space (aligned with light direction)
    AABox camAABoxLS;
    for (const Vector3 &camFrustumPointWS : camFrustumPointsWS)
    {
        Vector3 camFrustumPointLS =
            worldToLight.TransformedPoint(camFrustumPointWS);
        camAABoxLS.AddPoint(camFrustumPointLS);
    }

    // Get scene AABox
    const AABox sceneAABox = GetShadowCastersAABox(shadowCastersRenderers);

    // Extend light box in y and z back and forth, so that we can intersect in
    // the next step
    float sceneDiagonalLength = sceneAABox.GetDiagonal().Length();
    float padding = sceneDiagonalLength * 0.05f;
    const Vector3 Ext(padding, padding, sceneDiagonalLength * 1.05f);
    AABox extCamAABoxLS(camAABoxLS.GetMin() - Ext, camAABoxLS.GetMax() + Ext);

    // Now, intersect the very long top, bot, left and right shadow map ortho
    // box with
    // the scene AABBox, so that we can determine the optimal near and far
    // distances
    // for our shadow map ortho box! (so that all needed shadow casters are
    // inside)
    Quad extCamAABoxTopQuadWS = lightToWorld * extCamAABoxLS.GetTopQuad();
    Quad extCamAABoxBotQuadWS = lightToWorld * extCamAABoxLS.GetBotQuad();
    Quad extCamAABoxLeftQuadWS = lightToWorld * extCamAABoxLS.GetLeftQuad();
    Quad extCamAABoxRightQuadWS = lightToWorld * extCamAABoxLS.GetRightQuad();
    Quad extCamAABoxFrontQuadWS = lightToWorld * extCamAABoxLS.GetFrontQuad();
    Quad extCamAABoxBackQuadWS = lightToWorld * extCamAABoxLS.GetBackQuad();

    // Get scene AABBox and intersect with all extended quads!
    Array<Vector3> extCamAABoxSceneBoxIntersectionsWS =
        Geometry::IntersectBoxBox({{extCamAABoxTopQuadWS,
                                    extCamAABoxBotQuadWS,
                                    extCamAABoxLeftQuadWS,
                                    extCamAABoxRightQuadWS,
                                    extCamAABoxFrontQuadWS,
                                    extCamAABoxBackQuadWS}},
                                  {{sceneAABox.GetTopQuad(),
                                    sceneAABox.GetBotQuad(),
                                    sceneAABox.GetLeftQuad(),
                                    sceneAABox.GetRightQuad(),
                                    sceneAABox.GetFrontQuad(),
                                    sceneAABox.GetBackQuad()}});

    // Make an array of all the points we want to have inside our shadow map
    // First of all, we want to have the whole camera frustum inside of it
    // And then, we want to have all the shadow casters that affect us too
    Array<Vector3> pointsToBeShadowMappedWS;
    pointsToBeShadowMappedWS.PushBack(extCamAABoxSceneBoxIntersectionsWS);

    AABox orthoBoxInLightSpace;
    for (const Vector3 &pointToBeShadowMappedWS : pointsToBeShadowMappedWS)
    {
        Vector3 pointToBeShadowMappedLS =
            worldToLight.TransformedPoint(pointToBeShadowMappedWS);
        orthoBoxInLightSpace.AddPoint(pointToBeShadowMappedLS);
    }

    return orthoBoxInLightSpace;
}

Matrix4 DirectionalLight::GetLightToWorldMatrix() const
{
    const Transform *t = GetGameObject()->GetTransform();

    Matrix4 lightToWorld;
    lightToWorld[0] = Vector4(t->GetRight().NormalizedSafe(), 0);
    lightToWorld[1] = Vector4(t->GetUp().NormalizedSafe(), 0);
    lightToWorld[2] = Vector4(-t->GetForward().NormalizedSafe(), 0);
    lightToWorld[3] = Vector4(0, 0, 0, 1);
    return lightToWorld;
}
