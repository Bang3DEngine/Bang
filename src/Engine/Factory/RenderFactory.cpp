#include "Bang/RenderFactory.h"

#include "Bang/GL.h"
#include "Bang/Mesh.h"
#include "Bang/Math.h"
#include "Bang/Rect.h"
#include "Bang/AABox.h"
#include "Bang/Color.h"
#include "Bang/Scene.h"
#include "Bang/AARect.h"
#include "Bang/Camera.h"
#include "Bang/GBuffer.h"
#include "Bang/GEngine.h"
#include "Bang/Vector3.h"
#include "Bang/Vector4.h"
#include "Bang/Renderer.h"
#include "Bang/Material.h"
#include "Bang/Transform.h"
#include "Bang/Texture2D.h"
#include "Bang/Quaternion.h"
#include "Bang/GameObject.h"
#include "Bang/MeshFactory.h"
#include "Bang/MeshRenderer.h"
#include "Bang/LineRenderer.h"
#include "Bang/SceneManager.h"
#include "Bang/ShaderProgram.h"
#include "Bang/MaterialFactory.h"
#include "Bang/GameObjectFactory.h"
#include "Bang/ShaderProgramFactory.h"

USING_NAMESPACE_BANG

RenderFactory::RenderFactory()
{
    m_gizmosGo = GameObjectFactory::CreateGameObject();
    m_gizmosGo->SetName("Gizmos");

    m_boxMesh = Resources::Clone<Mesh>(MeshFactory::GetCube().Get());
    m_planeMesh = Resources::Clone<Mesh>(MeshFactory::GetUIPlane().Get());
    m_sphereMesh = Resources::Clone<Mesh>(MeshFactory::GetSphere().Get());
    m_outlineShaderProgram.Set( ShaderProgramFactory::Get(
                ShaderProgramFactory::GetScreenPassVertexShaderPath(),
                EPATH("Shaders/GizmosOutline.frag")) );

    m_lineRenderer = m_gizmosGo->AddComponent<LineRenderer>();
    m_meshRenderer = m_gizmosGo->AddComponent<MeshRenderer>();
    m_renderers    = m_gizmosGo->GetComponents<Renderer>();

    for (Renderer *rend : m_renderers)
    {
        rend->SetMaterial(MaterialFactory::GetGizmosUnLightedOverlay().Get());
    }

    m_gizmosGo->Start();
    m_gizmosGo->GetHideFlags().SetOn(HideFlag::DONT_SERIALIZE);
}

RenderFactory::~RenderFactory()
{
    GameObject::Destroy(m_gizmosGo);
}

void RenderFactory::RenderCustomMesh(Mesh *m,
                                     const RenderFactory::Parameters &params)
{
    RenderFactory *rf = RenderFactory::GetInstance(); if (!rf) { return; }

    RenderFactory::Parameters paramsCpy = params;
    paramsCpy.viewProjMode = GL::ViewProjMode::WORLD;
    rf->m_meshRenderer->SetMesh(m);

    Render(rf->m_meshRenderer, paramsCpy);
}


void RenderFactory::RenderSimpleBox(const AABox &b,
                                    const RenderFactory::Parameters &params)
{
    RenderFactory *rf = RenderFactory::GetInstance(); if (!rf) { return; }
    const Quaternion &r = params.rotation;
    const Vector3& bMin = b.GetMin();
    const Vector3& bMax = b.GetMax();

    RenderLine(r * Vector3(bMin.x, bMin.y, bMin.z),
               r * Vector3(bMax.x, bMin.y, bMin.z),
               params);
    RenderLine(r * Vector3(bMin.x, bMin.y, bMin.z),
               r * Vector3(bMin.x, bMax.y, bMin.z),
               params);
    RenderLine(r * Vector3(bMin.x, bMin.y, bMin.z),
               r * Vector3(bMin.x, bMin.y, bMax.z),
               params);

    RenderLine(r * Vector3(bMax.x, bMin.y, bMin.z),
               r * Vector3(bMax.x, bMax.y, bMin.z),
               params);
    RenderLine(r * Vector3(bMax.x, bMin.y, bMin.z),
               r * Vector3(bMax.x, bMin.y, bMax.z),
               params);

    RenderLine(r * Vector3(bMin.x, bMax.y, bMin.z),
               r * Vector3(bMax.x, bMax.y, bMin.z),
               params);
    RenderLine(r * Vector3(bMin.x, bMax.y, bMin.z),
               r * Vector3(bMin.x, bMax.y, bMax.z),
               params);

    RenderLine(r * Vector3(bMin.x, bMin.y, bMax.z),
               r * Vector3(bMax.x, bMin.y, bMax.z),
               params);
    RenderLine(r * Vector3(bMin.x, bMin.y, bMax.z),
               r * Vector3(bMin.x, bMax.y, bMax.z),
               params);

    RenderLine(r * Vector3(bMin.x, bMax.y, bMax.z),
               r * Vector3(bMax.x, bMax.y, bMax.z),
               params);
    RenderLine(r * Vector3(bMax.x, bMin.y, bMax.z),
               r * Vector3(bMax.x, bMax.y, bMax.z),
               params);
    RenderLine(r * Vector3(bMax.x, bMax.y, bMin.z),
               r * Vector3(bMax.x, bMax.y, bMax.z),
               params);
}

void RenderFactory::RenderBox(const AABox &b,
                              const RenderFactory::Parameters &params)
{
    RenderFactory *rf = RenderFactory::GetInstance(); if (!rf) { return; }
    rf->m_meshRenderer->SetMesh(rf->m_boxMesh.Get());

    RenderFactory::Parameters paramsCpy = params;
    paramsCpy.viewProjMode = GL::ViewProjMode::WORLD;
    paramsCpy.position = b.GetCenter();
    paramsCpy.scale *= b.GetSize();

    rf->Render(rf->m_meshRenderer, paramsCpy);
}

void RenderFactory::RenderRectNDC(const AARect &r,
                                  const RenderFactory::Parameters &params)
{
    RenderFactory::RenderRectNDC({{r.GetMinXMinY(), r.GetMaxXMinY(),
                                   r.GetMaxXMaxY(), r.GetMinXMaxY()}},
                                 params);
}

void RenderFactory::RenderRectNDC(const RectPoints &rectPointsNDC,
                                  const RenderFactory::Parameters &params)
{
    RenderViewportLineNDC(rectPointsNDC[0], rectPointsNDC[1], params);
    RenderViewportLineNDC(rectPointsNDC[1], rectPointsNDC[2], params);
    RenderViewportLineNDC(rectPointsNDC[2], rectPointsNDC[3], params);
    RenderViewportLineNDC(rectPointsNDC[3], rectPointsNDC[0], params);
}

void RenderFactory::RenderRect(const RectPoints &rectPoints,
                               const RenderFactory::Parameters &params)
{
    RenderRectNDC({{GL::FromViewportPointToViewportPointNDC(rectPoints[0]),
                    GL::FromViewportPointToViewportPointNDC(rectPoints[1]),
                    GL::FromViewportPointToViewportPointNDC(rectPoints[2]),
                    GL::FromViewportPointToViewportPointNDC(rectPoints[3])}},
                  params);
}

void RenderFactory::RenderRect(const Rect &r,
                               const RenderFactory::Parameters &params)
{
    RenderRect({{r.GetLeftBot(), r.GetRightBot(),
                 r.GetLeftTop(), r.GetRightTop()}},
               params);
}

void RenderFactory::RenderFillRect(const AARect &r,
                                   const RenderFactory::Parameters &params)
{
    RenderFactory *rf = RenderFactory::GetInstance(); if (!rf) { return; }
    rf->m_meshRenderer->SetMesh(rf->m_planeMesh.Get());

    RenderFactory::Parameters paramsCpy = params;
    paramsCpy.viewProjMode = GL::ViewProjMode::CANVAS;
    paramsCpy.position = Vector3(r.GetCenter(), 0);
    paramsCpy.scale = Vector3(r.GetSize(), 1);

    rf->Render(rf->m_meshRenderer, paramsCpy);
}

void RenderFactory::RenderIcon(Texture2D *texture,
                               bool billboard,
                               const RenderFactory::Parameters &params)
{
    RenderFactory *rf = RenderFactory::GetInstance(); if (!rf) { return; }
    rf->m_meshRenderer->SetMesh(rf->m_planeMesh.Get());

    RenderFactory::Parameters paramsCpy = params;
    paramsCpy.wireframe = false;
    paramsCpy.receivesLighting = false;
    if (billboard)
    {
        Camera *cam = Camera::GetActive();

        Vector3 camPos = cam->GetGameObject()->GetTransform()->GetPosition();
        float distScale = 1.0f;
        if (cam->GetProjectionMode() == Camera::ProjectionMode::PERSPECTIVE)
        {
           distScale = Vector3::Distance(camPos, paramsCpy.position);
        }

        paramsCpy.scale *= distScale * 0.5f;
        paramsCpy.rotation = Quaternion::LookDirection(
                                cam->GetGameObject()->GetTransform()->GetForward(),
                                cam->GetGameObject()->GetTransform()->GetUp());
    }
    paramsCpy.texture.Set( texture );
    rf->Render(rf->m_meshRenderer, paramsCpy);
}

void RenderFactory::RenderViewportIcon(Texture2D *texture,
                                       const AARect &winRect,
                                       const RenderFactory::Parameters &params)
{
    RenderFactory *rf = RenderFactory::GetInstance(); if (!rf) { return; }
    rf->m_meshRenderer->SetMesh(rf->m_planeMesh.Get());

    RenderFactory::Parameters paramsCpy = params;
    paramsCpy.viewProjMode = GL::ViewProjMode::CANVAS;
    paramsCpy.position = Vector3(winRect.GetCenter(), 0);
    paramsCpy.scale = Vector3(winRect.GetSize(), 1);
    paramsCpy.wireframe = false;
    paramsCpy.receivesLighting = false;

    rf->m_meshRenderer->GetActiveMaterial()->SetAlbedoTexture(texture);
    rf->Render(rf->m_meshRenderer, paramsCpy);
}

void RenderFactory::RenderLine(const Vector3 &origin,
                               const Vector3 &destiny,
                               const RenderFactory::Parameters &params)
{
    RenderFactory *rf = RenderFactory::GetInstance(); if (!rf) { return; }

    RenderFactory::Parameters paramsCpy = params;
    paramsCpy.viewProjMode = GL::ViewProjMode::WORLD;

    rf->m_lineRenderer->SetPoints( {origin, destiny} );
    rf->Render(rf->m_lineRenderer, paramsCpy);
}

void RenderFactory::RenderBillboardCircle(float radius,
                                          const RenderFactory::Parameters &params,
                                          int numSegments)
{
    RenderFactory *rf = RenderFactory::GetInstance(); if (!rf) { return; }

    Camera *cam = Camera::GetActive();
    Vector3 camPos = cam ? cam->GetGameObject()->GetTransform()->GetPosition() :
                           Vector3::Zero;

    RenderFactory::Parameters paramsCpy = params;
    paramsCpy.rotation =
      Quaternion::LookDirection((camPos - paramsCpy.position).NormalizedSafe());
    RenderFactory::RenderCircle(radius, paramsCpy, numSegments);
}

void RenderFactory::RenderCircle(float radius,
                                 const RenderFactory::Parameters &params,
                                 int numSegments)
{
    RenderFactory *rf = RenderFactory::GetInstance(); if (!rf) { return; }

    Array<Vector3> circlePoints;
    for (int i = 0; i < numSegments; ++i)
    {
        float angle = ((2 * Math::Pi) / numSegments) * i;
        Vector3 point = Vector3(Math::Cos(angle) * radius,
                                Math::Sin(angle) * radius,
                                0.0f);
        circlePoints.PushBack(point);
    }

    Array<Vector3> circleLinePoints;
    for (int i = 0; i < numSegments; ++i)
    {
        Vector3 p0 = circlePoints[i];
        Vector3 p1 = circlePoints[(i+1) % numSegments];
        circleLinePoints.PushBack(p0);
        circleLinePoints.PushBack(p1);
    }

    RenderFactory::Parameters paramsCpy = params;
    paramsCpy.viewProjMode = GL::ViewProjMode::WORLD;

    rf->m_lineRenderer->SetPoints(circleLinePoints);
    rf->Render(rf->m_lineRenderer, paramsCpy);
}

void RenderFactory::RenderViewportLineNDC(const Vector2 &origin,
                                          const Vector2 &destiny,
                                          const RenderFactory::Parameters &params)
{
    RenderFactory *rf = RenderFactory::GetInstance(); if (!rf) { return; }
    Vector2 originVP  = GL::FromViewportPointNDCToViewportPoint(origin);
    Vector2 destinyVP = GL::FromViewportPointNDCToViewportPoint(destiny);

    RenderFactory::Parameters paramsCpy = params;
    paramsCpy.viewProjMode = GL::ViewProjMode::CANVAS;

    rf->m_lineRenderer->SetPoints( {Vector3(originVP.x,  originVP.y,  0),
                                   Vector3(destinyVP.x, destinyVP.y, 0)} );
    rf->Render(rf->m_lineRenderer, paramsCpy);
}

void RenderFactory::RenderRay(const Vector3 &origin,
                              const Vector3 &rayDir,
                              const RenderFactory::Parameters &params)
{
    RenderFactory::RenderLine(origin, origin + rayDir, params);
}

void RenderFactory::RenderSphere(float radius,
                                 const RenderFactory::Parameters &params)
{
    RenderFactory *rf = RenderFactory::GetInstance(); if (!rf) { return; }
    rf->m_meshRenderer->SetMesh(rf->m_sphereMesh.Get());

    RenderFactory::Parameters paramsCpy = params;
    paramsCpy.scale = Vector3(radius);
    paramsCpy.viewProjMode = GL::ViewProjMode::WORLD;

    rf->Render(rf->m_meshRenderer, paramsCpy);
}

void RenderFactory::RenderSimpleSphere(float radius,
                                       bool withOutline,
                                       const RenderFactory::Parameters &params,
                                       int numLoopsVertical,
                                       int numLoopsHorizontal,
                                       int numCircleSegments)
{
    RenderFactory *rf = RenderFactory::GetInstance(); if (!rf) { return; }

    RenderFactory::Parameters paramsCpy = params;
    paramsCpy.viewProjMode = GL::ViewProjMode::WORLD;

    const float angleAdvVertical = (Math::Pi / numLoopsVertical);
    for (int i = 0; i < numLoopsVertical; ++i)
    {
        paramsCpy.rotation =
            Quaternion::AngleAxis(Math::Pi / 2, Vector3::Right) *
            Quaternion::AngleAxis(angleAdvVertical * i, Vector3::Right);
        RenderFactory::RenderCircle(radius, paramsCpy, numCircleSegments);
    }

    const float angleAdvHorizontal = (Math::Pi / numLoopsHorizontal);
    for (int i = 0; i < numLoopsHorizontal; ++i)
    {
        paramsCpy.rotation =
            Quaternion::AngleAxis(angleAdvHorizontal * i, Vector3::Up);
        RenderFactory::RenderCircle(radius, paramsCpy, numCircleSegments);
    }

    if (withOutline)
    {
        for (Renderer *r : rf->m_renderers) { r->SetEnabled(false); }
        paramsCpy.scale = Vector3(radius);
        rf->m_meshRenderer->SetEnabled(true);
        rf->m_meshRenderer->SetMesh( rf->m_sphereMesh.Get() );
        RenderFactory::ApplyRenderParameters(rf->m_meshRenderer, paramsCpy);
        RenderFactory::RenderOutline( rf->m_gizmosGo, paramsCpy );
        for (Renderer *r : rf->m_renderers) { r->SetEnabled(true); }
    }
}

void RenderFactory::RenderOutline(GameObject *gameObject,
                                  const RenderFactory::Parameters &params,
                                  float alphaDepthOnFade)
{
    RenderFactory *rf = RenderFactory::GetInstance(); if (!rf) { return; }

    // Save state
    GL::Push(GL::Pushable::COLOR_MASK);
    GL::Push(GL::Pushable::DEPTH_STATES);
    GL::Push(GL::Pushable::STENCIL_STATES);
    GL::Push(GL::BindTarget::SHADER_PROGRAM);
    GL::Push(GL::Pushable::FRAMEBUFFER_AND_READ_DRAW_ATTACHMENTS);

    GBuffer *gbuffer = GEngine::GetActiveGBuffer();
    if (gbuffer)
    {
        // Save before drawing the first pass, because it could happen that it
        // draws to gizmos and change the gizmos color and thickness.

        gbuffer->PushDrawAttachments();
        gbuffer->PushDepthStencilTexture();
        gbuffer->Bind();
        gbuffer->SetOverlayDepthStencil();

        // Render depth
        gbuffer->SetDrawBuffers({});
        GL::ClearDepthBuffer(1);
        GL::SetDepthMask(true);
        GL::SetDepthFunc(GL::Function::ALWAYS);
        GL::SetStencilOp(GL::StencilOperation::KEEP);
        GL::SetColorMask(false, false, false, false);

        GEngine::GetInstance()->SetReplacementMaterial(
                                 MaterialFactory::GetDefaultUnLighted().Get());
        GEngine::GetInstance()->RenderWithPass(gameObject, RenderPass::SCENE);
        GEngine::GetInstance()->SetReplacementMaterial(nullptr);

        // Render outline
        GL::SetDepthMask(false);
        GL::SetDepthFunc(GL::Function::ALWAYS);
        GL::SetColorMask(true, true, true, true);

        ShaderProgram *sp = rf->m_outlineShaderProgram.Get();
        sp->Bind();
        sp->SetColor("B_OutlineColor", params.color);
        sp->SetInt("B_OutlineThickness", params.thickness);
        sp->SetFloat("B_AlphaFadeOnDepth", alphaDepthOnFade);
        GBuffer *gbuffer = GEngine::GetActiveGBuffer();
        if (gbuffer)
        {
            sp->SetTexture("B_SceneDepthTexture",
                           gbuffer->GetSceneDepthStencilTexture(), false);
        }

        gbuffer->SetColorDrawBuffer();
        gbuffer->ApplyPass(sp, false);

        GL::SetDepthMask(true);
        GL::ClearDepthBuffer(1);

        gbuffer->PopDepthStencilTexture();
        gbuffer->PopDrawAttachments();
    }

    GL::Pop(GL::Pushable::FRAMEBUFFER_AND_READ_DRAW_ATTACHMENTS);
    GL::Pop(GL::BindTarget::SHADER_PROGRAM);
    GL::Pop(GL::Pushable::STENCIL_STATES);
    GL::Pop(GL::Pushable::DEPTH_STATES);
    GL::Pop(GL::Pushable::COLOR_MASK);
}

void RenderFactory::RenderFrustum(const Vector3 &forward,
                                  const Vector3 &up,
                                  const Vector3 &origin,
                                  float zNear,
                                  float zFar,
                                  float fovDegrees,
                                  float aspectRatio,
                                  const RenderFactory::Parameters &params)
{
    const Vector3 &c = origin;
    const Vector3 right = Vector3::Cross(forward, up).Normalized();

    const float fovH = Math::DegToRad(fovDegrees) / 2.0f;

    Vector3 nearPlaneCenter = c + forward * zNear;
    Vector3 farPlaneCenter  = c + forward * zFar;

    float nearHeight2 = zNear * Math::Tan(fovH);
    float nearWidth2 = nearHeight2 * aspectRatio;
    float farHeight2  = zFar  * Math::Tan(fovH);
    float farWidth2  = farHeight2 * aspectRatio;

    Vector3 nearUpLeft    = nearPlaneCenter - right * nearWidth2 + up * nearHeight2;
    Vector3 nearUpRight   = nearPlaneCenter + right * nearWidth2 + up * nearHeight2;
    Vector3 nearDownRight = nearPlaneCenter + right * nearWidth2 - up * nearHeight2;
    Vector3 nearDownLeft  = nearPlaneCenter - right * nearWidth2 - up * nearHeight2;

    Vector3 farUpLeft     = farPlaneCenter - right * farWidth2 + up * farHeight2;
    Vector3 farUpRight    = farPlaneCenter + right * farWidth2 + up * farHeight2;
    Vector3 farDownRight  = farPlaneCenter + right * farWidth2 - up * farHeight2;
    Vector3 farDownLeft   = farPlaneCenter - right * farWidth2 - up * farHeight2;

    // Near plane
    RenderFactory::RenderLine(nearUpLeft   , nearUpRight,   params);
    RenderFactory::RenderLine(nearUpRight  , nearDownRight, params);
    RenderFactory::RenderLine(nearDownRight, nearDownLeft,  params);
    RenderFactory::RenderLine(nearDownLeft , nearUpLeft,    params);

    // Far plane
    RenderFactory::RenderLine(farUpLeft   , farUpRight,   params);
    RenderFactory::RenderLine(farUpRight  , farDownRight, params);
    RenderFactory::RenderLine(farDownRight, farDownLeft,  params);
    RenderFactory::RenderLine(farDownLeft , farUpLeft,    params);

    // Projection lines
    RenderFactory::RenderLine(nearUpLeft   , farUpLeft,    params);
    RenderFactory::RenderLine(nearUpRight  , farUpRight,   params);
    RenderFactory::RenderLine(nearDownRight, farDownRight, params);
    RenderFactory::RenderLine(nearDownLeft , farDownLeft,  params);
}

void RenderFactory::RenderPoint(const Vector3 &point,
                                const RenderFactory::Parameters &params)
{
    RenderFactory *rf = RenderFactory::GetInstance(); if (!rf) { return; }

    RH<Mesh> rhm = Resources::Create<Mesh>();
    Mesh *m = rhm.Get();
    m->SetPositionsPool( {point} );
    m->UpdateGeometry();

    rf->m_meshRenderer->SetMesh(m);
    rf->m_meshRenderer->SetRenderPrimitive(GL::Primitive::POINTS);

    Render(rf->m_meshRenderer, params);

    rf->m_meshRenderer->SetRenderPrimitive(GL::Primitive::TRIANGLES);
}

GameObject *RenderFactory::GetGameObject() const
{
    return m_gizmosGo;
}

void RenderFactory::Render(Renderer *rend,
                           const RenderFactory::Parameters &params)
{
    ApplyRenderParameters( rend, params );
    rend->OnRender( rend->GetMaterial()->GetRenderPass() );
}

RenderFactory* RenderFactory::GetInstance()
{
    Scene *scene = SceneManager::GetActiveScene();
    return scene ? scene->GetGizmos() : nullptr;
}

void RenderFactory::ApplyRenderParameters(Renderer *rend,
                                          const RenderFactory::Parameters &params)
{
    if (rend)
    {
        Material *mat = rend->GetMaterial();
        mat->SetAlbedoColor(params.color);
        mat->SetReceivesLighting(params.receivesLighting);
        mat->SetRenderWireframe(params.wireframe);
        mat->SetCullFace(params.cullFace);
        mat->SetLineWidth(params.thickness);
        mat->SetAlbedoTexture(params.texture.Get());

        rend->SetViewProjMode(params.viewProjMode);
    }

    RenderFactory *rf = RenderFactory::GetInstance();
    rf->GetGameObject()->GetTransform()->SetPosition(params.position);
    rf->GetGameObject()->GetTransform()->SetRotation(params.rotation);
    rf->GetGameObject()->GetTransform()->SetLocalScale(params.scale);
}