#include "Bang/DirectionalLight.h"

#include "Bang/Quad.h"
#include "Bang/AABox.h"
#include "Bang/Ray2D.h"
#include "Bang/Scene.h"
#include "Bang/Camera.h"
#include "Bang/Gizmos.h"
#include "Bang/GEngine.h"
#include "Bang/Polygon.h"
#include "Bang/Segment.h"
#include "Bang/XMLNode.h"
#include "Bang/Geometry.h"
#include "Bang/Triangle.h"
#include "Bang/Texture2D.h"
#include "Bang/Segment2D.h"
#include "Bang/Resources.h"
#include "Bang/Transform.h"
#include "Bang/GLUniforms.h"
#include "Bang/Framebuffer.h"
#include "Bang/TextureFactory.h"
#include "Bang/ShadowMapper.h"
#include "Bang/ShaderProgram.h"
#include "Bang/DebugRenderer.h"
#include "Bang/ShaderProgramFactory.h"

USING_NAMESPACE_BANG

DirectionalLight::DirectionalLight()
{
    m_shadowMapFramebuffer = new Framebuffer(1,1);
    m_shadowMapFramebuffer->CreateAttachmentTex2D(GL::Attachment::DEPTH,
                                             GL::ColorFormat::DEPTH16);

    GLId prevBoundTex = GL::GetBoundId(GetShadowMapTexture()->GetGLBindTarget());
    GetShadowMapTexture()->Bind();
    GetShadowMapTexture()->SetFilterMode(GL::FilterMode::BILINEAR);
    GL::TexParameteri( GetShadowMapTexture()->GetTextureTarget(),
                       GL::TexParameter::TEXTURE_COMPARE_MODE,
                       GL_COMPARE_REF_TO_TEXTURE );
    GL::Bind(GetShadowMapTexture()->GetGLBindTarget(), prevBoundTex);

    SetLightScreenPassShaderProgram(
                ShaderProgramFactory::GetDirectionalLightScreenPass());
}

DirectionalLight::~DirectionalLight()
{
    delete m_shadowMapFramebuffer;
}

void DirectionalLight::RenderShadowMaps_()
{
    // Save previous state
    AARecti prevVP = GL::GetViewportRect();
    const Matrix4 &prevModel = GLUniforms::GetModelMatrix();
    const Matrix4 &prevView  = GLUniforms::GetViewMatrix();
    const Matrix4 &prevProj  = GLUniforms::GetProjectionMatrix();
    GLId prevBoundFB = GL::GetBoundId(m_shadowMapFramebuffer->GetGLBindTarget());

    // Bind and resize shadow map framebuffer
    const Vector2i& shadowMapSize = GetShadowMapSize();
    m_shadowMapFramebuffer->Bind();
    m_shadowMapFramebuffer->Resize(shadowMapSize.x, shadowMapSize.y);

    // Set up viewport
    GL::SetViewport(0, 0, shadowMapSize.x, shadowMapSize.y);

    // Set up shadow map matrices
    Scene *scene = GetGameObject()->GetScene();
    Matrix4 shadowMapViewMatrix, shadowMapProjMatrix;
    GetWorldToShadowMapMatrices(scene, &shadowMapViewMatrix, &shadowMapProjMatrix);
    GLUniforms::SetModelMatrix(Matrix4::Identity);
    GLUniforms::SetViewMatrix( shadowMapViewMatrix );
    GLUniforms::SetProjectionMatrix( shadowMapProjMatrix );
    m_lastUsedShadowMapViewProj = shadowMapProjMatrix * shadowMapViewMatrix;

    // Render shadow map into framebuffer
    GL::ClearDepthBuffer(1.0f);
    GL::SetColorMask(false, false, false, false);
    GL::SetDepthFunc(GL::Function::LEQUAL);
    GL::SetDepthMask(true);

    const List<GameObject*> shadowCasters = GetActiveSceneShadowCasters();
    for (GameObject *shadowCaster : shadowCasters)
    {
        GEngine::GetInstance()->RenderWithPass(shadowCaster, RenderPass::SCENE,
                                             false);
    }

    // Restore previous state
    GL::SetViewport(prevVP);
    GL::SetColorMask(true, true, true, true);
    GLUniforms::SetModelMatrix(prevModel);
    GLUniforms::SetViewMatrix(prevView);
    GLUniforms::SetProjectionMatrix(prevProj);
    GL::Bind(m_shadowMapFramebuffer->GetGLBindTarget(), prevBoundFB);
}

void DirectionalLight::SetUniformsBeforeApplyingLight(ShaderProgram* sp) const
{
    Light::SetUniformsBeforeApplyingLight(sp);

    ASSERT(GL::IsBound(sp))
    sp->SetFloat("B_ShadowDistance", GetShadowDistance(), false);
    sp->SetMatrix4("B_WorldToShadowMapMatrix", m_lastUsedShadowMapViewProj, false);
}

void DirectionalLight::SetShadowDistance(float shadowDistance)
{
    m_shadowDistance = shadowDistance;
}

float DirectionalLight::GetShadowDistance() const
{
    return m_shadowDistance;
}

Texture2D *DirectionalLight::GetShadowMapTexture() const
{
    return m_shadowMapFramebuffer->GetAttachmentTex2D(GL::Attachment::DEPTH);
}

void DirectionalLight::CloneInto(ICloneable *clone) const
{
    Light::CloneInto(clone);

    DirectionalLight *dl = SCAST<DirectionalLight*>(clone);
    dl->SetShadowDistance( GetShadowDistance() );
}

void DirectionalLight::GetWorldToShadowMapMatrices(Scene *scene,
                                                   Matrix4 *viewMatrix,
                                                   Matrix4 *projMatrix) const
{
    // The ortho box will be the AABox in light space of the AABox of the
    // scene in world space
    AABox orthoBoxInLightSpace = GetShadowMapOrthoBox(scene);
    Vector3 orthoBoxExtents = orthoBoxInLightSpace.GetExtents();
    Matrix4 lightToWorld = GetLightToWorldMatrix();
    Vector3 fwd = lightToWorld.TransformedVector(Vector3::Forward);
    Vector3 up  = lightToWorld.TransformedVector(Vector3::Up);

    Vector3 orthoBoxCenterWorld =
         lightToWorld.TransformedPoint( orthoBoxInLightSpace.GetCenter() );

    *viewMatrix = Matrix4::LookAt(orthoBoxCenterWorld,
                                  orthoBoxCenterWorld + fwd,
                                  up);

    *projMatrix = Matrix4::Ortho(-orthoBoxExtents.x, orthoBoxExtents.x,
                                 -orthoBoxExtents.y, orthoBoxExtents.y,
                                 -orthoBoxExtents.z, orthoBoxExtents.z);
}

Matrix4 DirectionalLight::GetShadowMapMatrix(Scene *scene) const
{
    Matrix4 shadowMapViewMatrix, shadowMapProjMatrix;
    GetWorldToShadowMapMatrices(scene, &shadowMapViewMatrix, &shadowMapProjMatrix);
    return shadowMapProjMatrix * shadowMapViewMatrix;
}

AABox DirectionalLight::GetShadowMapOrthoBox(Scene *scene) const
{
    // Adjust zFar so that we take into account shadow distance, and get our
    // camera frustum quads, then restore zFar
    Camera *cam = Camera::GetActive(); // Get active camera
    float prevZFar = cam->GetZFar();   // Save for later restore
    cam->SetZFar( Math::Min(prevZFar, GetShadowDistance()) ); // Shadow distance
    const Quad camTopQuad   = cam->GetFrustumTopQuad();    // Get top quad
    const Quad camBotQuad   = cam->GetFrustumBotQuad();    // Get bot quad
    const Quad camLeftQuad  = cam->GetFrustumLeftQuad();   // Get left quad
    const Quad camRightQuad = cam->GetFrustumRightQuad();  // Get right quad
    const std::array<Quad, 4> camQuads = {{camTopQuad, camBotQuad,
                                           camLeftQuad, camRightQuad}};
    cam->SetZFar(prevZFar); // Restore

    // Get all camera frustum points in world space
    Array<Vector3> camFrustumPointsWS;
    for (const Quad &quad : camQuads)
    {
        for (const Vector3 &p : quad.GetPoints())
        { camFrustumPointsWS.PushBack(p); }
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
        camAABoxLS.AddPoint( camFrustumPointLS );
    }

    // Get scene AABox
    const AABox sceneAABox = ShadowMapper::GetSceneCastersAABox(scene);

    // Extend light box in y and z back and forth, so that we can intersect in
    // the next step
    float sceneDiagonalLength = sceneAABox.GetDiagonal().Length();
    float padding = sceneDiagonalLength * 0.05f;
    const Vector3 Ext(padding, padding, sceneDiagonalLength * 1.05f);
    AABox extCamAABoxLS(camAABoxLS.GetMin() - Ext, camAABoxLS.GetMax() + Ext);

    // Now, intersect the very long top, bot, left and right shadow map ortho box with
    // the scene AABBox, so that we can determine the optimal near and far distances
    // for our shadow map ortho box! (so that all needed shadow casters are inside)
    Quad extCamAABoxTopQuadWS   = lightToWorld * extCamAABoxLS.GetTopQuad();
    Quad extCamAABoxBotQuadWS   = lightToWorld * extCamAABoxLS.GetBotQuad();
    Quad extCamAABoxLeftQuadWS  = lightToWorld * extCamAABoxLS.GetLeftQuad();
    Quad extCamAABoxRightQuadWS = lightToWorld * extCamAABoxLS.GetRightQuad();
    Quad extCamAABoxFrontQuadWS = lightToWorld * extCamAABoxLS.GetFrontQuad();
    Quad extCamAABoxBackQuadWS  = lightToWorld * extCamAABoxLS.GetBackQuad();

    // Get scene AABBox and intersect with all extended quads!
    Array<Vector3> extCamAABoxSceneBoxIntersectionsWS =
      Geometry::IntersectBoxBox({{extCamAABoxTopQuadWS,   extCamAABoxBotQuadWS,
                                  extCamAABoxLeftQuadWS,  extCamAABoxRightQuadWS,
                                  extCamAABoxFrontQuadWS, extCamAABoxBackQuadWS}},
                                {{sceneAABox.GetTopQuad(),   sceneAABox.GetBotQuad(),
                                  sceneAABox.GetLeftQuad(),  sceneAABox.GetRightQuad(),
                                  sceneAABox.GetFrontQuad(), sceneAABox.GetBackQuad()}});

    // Make an array of all the points we want to have inside our shadow map
    // First of all, we want to have the whole camera frustum inside of it
    // And then, we want to have all the shadow casters that affect us too

    /*
    float time = 20.0f;
    if (Input::GetKeyDown(Key::Q))
    {
        DebugRenderer::RenderQuad(extCamAABoxTopQuadWS,   Color::Blue, time, false, false, true, Color::Black);
        DebugRenderer::RenderQuad(extCamAABoxBotQuadWS,   Color::Blue, time, false, false, true, Color::Black);
        DebugRenderer::RenderQuad(extCamAABoxLeftQuadWS,  Color::Blue, time, false, false, true, Color::Black);
        DebugRenderer::RenderQuad(extCamAABoxRightQuadWS, Color::Blue, time, false, false, true, Color::Black);
        // DebugRenderer::RenderQuad(camTopQuad,   Color::LightBlue, time, false, false, true, Color::Black);
        // DebugRenderer::RenderQuad(camBotQuad,   Color::LightBlue, time, false, false, true, Color::Black);
        // DebugRenderer::RenderQuad(camLeftQuad,  Color::LightBlue, time, false, false, true, Color::Black);
        // DebugRenderer::RenderQuad(camRightQuad, Color::LightBlue, time, false, false, true, Color::Black);
        // DebugRenderer::RenderQuad(camAABoxLS.GetTopQuad(),   Color::Green, time, false, false, true, Color::Black);
        // DebugRenderer::RenderQuad(camAABoxLS.GetBotQuad(),   Color::Green, time, false, false, true, Color::Black);
        // DebugRenderer::RenderQuad(camAABoxLS.GetRightQuad(), Color::Green, time, false, false, true, Color::Black);
        // DebugRenderer::RenderQuad(camAABoxLS.GetLeftQuad(),  Color::Green, time, false, false, true, Color::Black);
        DebugRenderer::RenderAABox(sceneAABox, Color::Purple, time, 1.0f, true, false, true, Color::Black);

        for (const Vector3 &intPoint : extCamAABoxSceneBoxIntersectionsWS)
        for (const Vector3 &intPoint : ints)
        {
            DebugRenderer::RenderPoint(intPoint, Color::Red, time, 20.0f, true);
        }
        // DebugRenderer::RenderQuad(orthoBoxLeftQuadWorldSpace, Color::Blue, time, false, false, true);
        // DebugRenderer::RenderQuad(orthoBoxRightQuadWorldSpace, Color::Blue, time, false, false, true);
    }
    */

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
    lightToWorld[0] = Vector4(t->GetRight().NormalizedSafe(),    0);
    lightToWorld[1] = Vector4(t->GetUp().NormalizedSafe(),       0);
    lightToWorld[2] = Vector4(-t->GetForward().NormalizedSafe(), 0);
    lightToWorld[3] = Vector4(0, 0, 0, 1);
    return lightToWorld;
}

void DirectionalLight::OnRender(RenderPass rp)
{
    Component::OnRender(rp);

    // Gizmo rendering
    if (rp == RenderPass::OVERLAY)
    {
        Gizmos::Reset();
        Gizmos::SetColor(GetColor().WithAlpha(1.0f));
        Gizmos::SetSelectable(GetGameObject());
        Gizmos::SetPosition( GetGameObject()->GetTransform()->GetPosition() );
        Gizmos::SetScale( Vector3(0.1f) );
        Gizmos::RenderIcon( TextureFactory::GetSunIcon().Get(), true );
    }
}

void DirectionalLight::ImportXML(const XMLNode &xmlInfo)
{
    Light::ImportXML(xmlInfo);

    if (xmlInfo.Contains("ShadowDistance"))
    { SetShadowDistance(xmlInfo.Get<float>("ShadowDistance")); }
}

void DirectionalLight::ExportXML(XMLNode *xmlInfo) const
{
    Light::ExportXML(xmlInfo);

    xmlInfo->Set("ShadowDistance", GetShadowDistance());
}
