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

    m_boxMesh = Resources::Clone<Mesh>(MeshFactory::GetCube());
    m_planeMesh = Resources::Clone<Mesh>(MeshFactory::GetUIPlane());
    m_sphereMesh = Resources::Clone<Mesh>(MeshFactory::GetSphere());
    m_outlineShaderProgram = ShaderProgramFactory::Get(
                ShaderProgramFactory::GetScreenPassVertexShaderPath(),
                EPATH("Shaders/GizmosOutline.frag"));

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

void RenderFactory::SetColor(const Color &color)
{
    RenderFactory *g = RenderFactory::GetInstance(); if (!g) { return; }
    for (Renderer *rend : g->m_renderers)
    {
        rend->GetMaterial()->SetAlbedoColor(color);
    }
}

void RenderFactory::SetCullFace(GL::CullFaceExt cullFace)
{
    RenderFactory *g = RenderFactory::GetInstance(); if (!g) { return; }
    for (Renderer *rend : g->m_renderers)
    {
        rend->GetMaterial()->SetCullFace(cullFace);
    }
}

void RenderFactory::SetPosition(const Vector3 &position)
{
    RenderFactory *g = RenderFactory::GetInstance(); if (!g) { return; }
    g->m_gizmosGo->GetTransform()->SetPosition(position);
}

void RenderFactory::SetRotation(const Quaternion &rotation)
{
    RenderFactory *g = RenderFactory::GetInstance(); if (!g) { return; }
    g->m_gizmosGo->GetTransform()->SetRotation(rotation);
}

void RenderFactory::SetScale(const Vector3 &scale)
{
    RenderFactory *g = RenderFactory::GetInstance(); if (!g) { return; }
    g->m_gizmosGo->GetTransform()->SetLocalScale(scale);
}

void RenderFactory::SetRenderPass(RenderPass rp)
{
    RenderFactory *g = RenderFactory::GetInstance(); if (!g) { return; }
    for (Renderer *rend : g->m_renderers)
    {
        rend->GetActiveMaterial()->SetRenderPass(rp);
    }
}

void RenderFactory::SetThickness(float thickness)
{
    RenderFactory *g = RenderFactory::GetInstance(); if (!g) { return; }
    for (Renderer *rend : g->m_renderers)
    {
        rend->GetMaterial()->SetLineWidth(thickness);
    }
}

void RenderFactory::SetRenderWireframe(bool wireframe)
{
    RenderFactory *g = RenderFactory::GetInstance(); if (!g) { return; }
    for (Renderer *rend : g->m_renderers)
    {
        rend->GetMaterial()->SetRenderWireframe(wireframe);
    }
}

void RenderFactory::SetReceivesLighting(bool receivesLighting)
{
    RenderFactory *g = RenderFactory::GetInstance(); if (!g) { return; }
    for (Renderer *rend : g->m_renderers)
    {
        rend->GetActiveMaterial()->SetReceivesLighting(receivesLighting);
    }
}

void RenderFactory::RenderCustomMesh(Mesh *m)
{
    RenderFactory *g = RenderFactory::GetInstance(); if (!g) { return; }
    g->m_meshRenderer->SetMesh(m);
    Render(g->m_meshRenderer);
}


void RenderFactory::RenderSimpleBox(const AABox &b)
{
    RenderFactory *g = RenderFactory::GetInstance(); if (!g) { return; }
    const Quaternion &r = g->m_gizmosGo->GetTransform()->GetLocalRotation();
    const Vector3& bMin = b.GetMin();
    const Vector3& bMax = b.GetMax();

    RenderLine(r * Vector3(bMin.x, bMin.y, bMin.z), r * Vector3(bMax.x, bMin.y, bMin.z));
    RenderLine(r * Vector3(bMin.x, bMin.y, bMin.z), r * Vector3(bMin.x, bMax.y, bMin.z));
    RenderLine(r * Vector3(bMin.x, bMin.y, bMin.z), r * Vector3(bMin.x, bMin.y, bMax.z));

    RenderLine(r * Vector3(bMax.x, bMin.y, bMin.z), r * Vector3(bMax.x, bMax.y, bMin.z));
    RenderLine(r * Vector3(bMax.x, bMin.y, bMin.z), r * Vector3(bMax.x, bMin.y, bMax.z));

    RenderLine(r * Vector3(bMin.x, bMax.y, bMin.z), r * Vector3(bMax.x, bMax.y, bMin.z));
    RenderLine(r * Vector3(bMin.x, bMax.y, bMin.z), r * Vector3(bMin.x, bMax.y, bMax.z));

    RenderLine(r * Vector3(bMin.x, bMin.y, bMax.z), r * Vector3(bMax.x, bMin.y, bMax.z));
    RenderLine(r * Vector3(bMin.x, bMin.y, bMax.z), r * Vector3(bMin.x, bMax.y, bMax.z));

    RenderLine(r * Vector3(bMin.x, bMax.y, bMax.z), r * Vector3(bMax.x, bMax.y, bMax.z));
    RenderLine(r * Vector3(bMax.x, bMin.y, bMax.z), r * Vector3(bMax.x, bMax.y, bMax.z));
    RenderLine(r * Vector3(bMax.x, bMax.y, bMin.z), r * Vector3(bMax.x, bMax.y, bMax.z));
}

void RenderFactory::RenderBox(const AABox &b)
{
    RenderFactory *g = RenderFactory::GetInstance(); if (!g) { return; }
    g->m_meshRenderer->SetMesh(g->m_boxMesh.Get());
    g->m_gizmosGo->GetTransform()->SetPosition(b.GetCenter());
    g->m_gizmosGo->GetTransform()->
       SetScale(g->m_gizmosGo->GetTransform()->GetScale() * b.GetSize());
    g->Render(g->m_meshRenderer);
}

void RenderFactory::RenderRectNDC(const AARect &r)
{
    RenderFactory::RenderRectNDC({{r.GetMinXMinY(), r.GetMaxXMinY(),
                            r.GetMaxXMaxY(), r.GetMinXMaxY()}});
}

void RenderFactory::RenderRectNDC(const RectPoints &rectPointsNDC)
{
    RenderViewportLineNDC(rectPointsNDC[0], rectPointsNDC[1]);
    RenderViewportLineNDC(rectPointsNDC[1], rectPointsNDC[2]);
    RenderViewportLineNDC(rectPointsNDC[2], rectPointsNDC[3]);
    RenderViewportLineNDC(rectPointsNDC[3], rectPointsNDC[0]);
}

void RenderFactory::RenderRect(const RectPoints &rectPoints)
{
    RenderRectNDC({{GL::FromViewportPointToViewportPointNDC(rectPoints[0]),
                    GL::FromViewportPointToViewportPointNDC(rectPoints[1]),
                    GL::FromViewportPointToViewportPointNDC(rectPoints[2]),
                    GL::FromViewportPointToViewportPointNDC(rectPoints[3])}});
}

void RenderFactory::RenderRect(const Rect &r)
{
    RenderRect({{r.GetLeftBot(), r.GetRightBot(),
                 r.GetLeftTop(), r.GetRightTop()}});
}

void RenderFactory::RenderFillRect(const AARect &r)
{
    RenderFactory *g = RenderFactory::GetInstance(); if (!g) { return; }
    g->m_meshRenderer->SetMesh(g->m_planeMesh.Get());

    RenderFactory::SetPosition( Vector3(r.GetCenter(), 0) );
    RenderFactory::SetScale( Vector3(r.GetSize(), 1) );

    g->m_meshRenderer->SetViewProjMode(GL::ViewProjMode::CANVAS);
    g->Render(g->m_meshRenderer);
}

void RenderFactory::RenderIcon(Texture2D *texture,
                        bool billboard)
{
    RenderFactory *g = RenderFactory::GetInstance(); if (!g) { return; }
    g->m_meshRenderer->SetMesh(g->m_planeMesh.Get());

    SetRenderWireframe(false);
    SetReceivesLighting(false);
    if (billboard)
    {
        Camera *cam = Camera::GetActive();

        Vector3 camPos = cam->GetGameObject()->GetTransform()->GetPosition();
        float distScale = 1.0f;
        if (cam->GetProjectionMode() == Camera::ProjectionMode::PERSPECTIVE)
        {
           Vector3 pos = g->m_gizmosGo->GetTransform()->GetPosition();
           distScale = Vector3::Distance(camPos, pos);
        }

        Vector3 scale = g->m_gizmosGo->GetTransform()->GetScale();
        g->m_gizmosGo->GetTransform()->SetScale(distScale * scale * 0.5f);

        g->m_gizmosGo->GetTransform()->LookInDirection(
                    cam->GetGameObject()->GetTransform()->GetForward(),
                    cam->GetGameObject()->GetTransform()->GetUp());
    }
    g->m_meshRenderer->GetActiveMaterial()->SetAlbedoTexture(texture);
    g->Render(g->m_meshRenderer);
}

void RenderFactory::RenderViewportIcon(Texture2D *texture,
                                const AARect &winRect)
{
    RenderFactory *g = RenderFactory::GetInstance(); if (!g) { return; }
    g->m_meshRenderer->SetMesh(g->m_planeMesh.Get());

    RenderFactory::SetPosition( Vector3(winRect.GetCenter(), 0) );
    RenderFactory::SetScale( Vector3(winRect.GetSize(), 1) );

    SetRenderWireframe(false);
    SetReceivesLighting(false);
    g->m_meshRenderer->GetActiveMaterial()->SetAlbedoTexture(texture);
    g->m_meshRenderer->SetViewProjMode(GL::ViewProjMode::CANVAS);
    g->Render(g->m_meshRenderer);
}

void RenderFactory::RenderLine(const Vector3 &origin, const Vector3 &destiny)
{
    RenderFactory *g = RenderFactory::GetInstance(); if (!g) { return; }
    g->m_lineRenderer->SetPoints( {origin, destiny} );

    g->m_gizmosGo->GetTransform()->SetPosition(Vector3::Zero);
    g->m_gizmosGo->GetTransform()->SetScale(Vector3::One);

    g->m_lineRenderer->SetViewProjMode(GL::ViewProjMode::WORLD);
    g->Render(g->m_lineRenderer);
}

void RenderFactory::RenderBillboardCircle(const Vector3 &origin, float radius,
                                   int numSegments)
{
    RenderFactory *g = RenderFactory::GetInstance(); if (!g) { return; }

    Camera *cam = Camera::GetActive();
    Vector3 camPos = cam ? cam->GetGameObject()->GetTransform()->GetPosition() :
                           Vector3::Zero;
    g->m_gizmosGo->GetTransform()->LookAt(camPos);
    RenderFactory::RenderCircle(origin, radius, numSegments);
}

void RenderFactory::RenderCircle(const Vector3 &origin, float radius, int numSegments)
{
    RenderFactory *g = RenderFactory::GetInstance(); if (!g) { return; }
    RenderFactory::SetPosition(origin);

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

    g->m_lineRenderer->SetPoints(circleLinePoints);
    g->Render(g->m_lineRenderer);
}

void RenderFactory::RenderViewportLineNDC(const Vector2 &origin, const Vector2 &destiny)
{
    RenderFactory *g = RenderFactory::GetInstance(); if (!g) { return; }
    Vector2 originVP  = GL::FromViewportPointNDCToViewportPoint(origin);
    Vector2 destinyVP = GL::FromViewportPointNDCToViewportPoint(destiny);
    g->m_lineRenderer->SetPoints( {Vector3(originVP.x,  originVP.y,  0),
                                   Vector3(destinyVP.x, destinyVP.y, 0)} );

    g->m_gizmosGo->GetTransform()->SetPosition(Vector3::Zero);
    g->m_gizmosGo->GetTransform()->SetScale(Vector3::One);

    g->m_lineRenderer->SetViewProjMode(GL::ViewProjMode::CANVAS);
    g->Render(g->m_lineRenderer);
}

void RenderFactory::RenderRay(const Vector3 &origin, const Vector3 &rayDir)
{
    RenderFactory::RenderLine(origin, origin + rayDir);
}

void RenderFactory::RenderSphere(const Vector3 &origin, float radius)
{
    RenderFactory *g = RenderFactory::GetInstance(); if (!g) { return; }
    g->m_meshRenderer->SetMesh(g->m_sphereMesh.Get());
    g->m_gizmosGo->GetTransform()->SetPosition(origin);
    g->m_gizmosGo->GetTransform()->SetScale(radius);
    g->Render(g->m_meshRenderer);
}

void RenderFactory::RenderSimpleSphere(const Vector3 &origin,
                                float radius,
                                bool withOutline,
                                int numLoopsVertical,
                                int numLoopsHorizontal,
                                int numCircleSegments)
{
    RenderFactory *g = RenderFactory::GetInstance(); if (!g) { return; }

    const float angleAdvVertical = (Math::Pi / numLoopsVertical);
    for (int i = 0; i < numLoopsVertical; ++i)
    {
        RenderFactory::SetRotation(
            Quaternion::AngleAxis(Math::Pi / 2, Vector3::Right) *
            Quaternion::AngleAxis(angleAdvVertical * i, Vector3::Right));
        RenderFactory::RenderCircle(origin, radius, numCircleSegments);
    }

    const float angleAdvHorizontal = (Math::Pi / numLoopsHorizontal);
    for (int i = 0; i < numLoopsHorizontal; ++i)
    {
        RenderFactory::SetRotation(
            Quaternion::AngleAxis(angleAdvHorizontal * i, Vector3::Up));
        RenderFactory::RenderCircle(origin, radius, numCircleSegments);
    }

    if (withOutline)
    {
        for (Renderer *r : g->m_renderers) { r->SetEnabled(false); }
        RenderFactory::SetPosition( origin );
        RenderFactory::SetRotation( Quaternion::Identity );
        RenderFactory::SetScale( Vector3(radius) );
        g->m_meshRenderer->SetEnabled(true);
        g->m_meshRenderer->SetMesh( g->m_sphereMesh.Get() );
        RenderFactory::RenderOutline( g->m_gizmosGo );
        for (Renderer *r : g->m_renderers) { r->SetEnabled(true); }
    }
}

void RenderFactory::RenderOutline(GameObject *gameObject,
                           float alphaDepthOnFade)
{
    RenderFactory *g = RenderFactory::GetInstance(); if (!g) { return; }

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
        const Color outlineColor = g->m_meshRenderer->GetActiveMaterial()->
                                   GetAlbedoColor();
        const float outlineThickness = g->m_meshRenderer->GetActiveMaterial()->
                                       GetLineWidth();

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

        ShaderProgram *sp = g->m_outlineShaderProgram.Get();
        sp->Bind();
        sp->SetColor("B_OutlineColor", outlineColor);
        sp->SetInt("B_OutlineThickness", outlineThickness);
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
                           float zNear, float zFar,
                           float fovDegrees, float aspectRatio)
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
    RenderFactory::RenderLine(nearUpLeft   , nearUpRight);
    RenderFactory::RenderLine(nearUpRight  , nearDownRight);
    RenderFactory::RenderLine(nearDownRight, nearDownLeft);
    RenderFactory::RenderLine(nearDownLeft , nearUpLeft);

    // Far plane
    RenderFactory::RenderLine(farUpLeft   , farUpRight);
    RenderFactory::RenderLine(farUpRight  , farDownRight);
    RenderFactory::RenderLine(farDownRight, farDownLeft);
    RenderFactory::RenderLine(farDownLeft , farUpLeft);

    // Projection lines
    RenderFactory::RenderLine(nearUpLeft   , farUpLeft);
    RenderFactory::RenderLine(nearUpRight  , farUpRight);
    RenderFactory::RenderLine(nearDownRight, farDownRight);
    RenderFactory::RenderLine(nearDownLeft , farDownLeft);
}

void RenderFactory::RenderPoint(const Vector3 &point)
{
    RenderFactory *g = RenderFactory::GetInstance(); if (!g) { return; }

    RH<Mesh> rhm = Resources::Create<Mesh>();
    Mesh *m = rhm.Get();
    m->SetPositionsPool( {point} );

    g->m_gizmosGo->GetTransform()->SetPosition(Vector3::Zero);
    g->m_meshRenderer->SetMesh(m);
    g->m_meshRenderer->SetRenderPrimitive(GL::Primitive::POINTS);

    Render(g->m_meshRenderer);

    g->m_meshRenderer->SetRenderPrimitive(GL::Primitive::TRIANGLES);
}

void RenderFactory::Reset()
{
    RenderFactory *g = RenderFactory::GetInstance(); if (!g) { return; }

    RenderFactory::SetCullFace(GL::CullFaceExt::BACK);
    RenderFactory::SetPosition(Vector3::Zero);
    RenderFactory::SetRotation(Quaternion::Identity);
    RenderFactory::SetScale(Vector3::One);
    RenderFactory::SetColor(Color::White);
    RenderFactory::SetThickness(1.0f);
    RenderFactory::SetReceivesLighting(false);
    RenderFactory::SetRenderWireframe(false);
    RenderFactory::SetRenderPass(RenderPass::OVERLAY);

    List<Renderer*> rends = g->m_gizmosGo->GetComponents<Renderer>();
    for (Renderer *rend : rends)
    {
        rend->SetViewProjMode(GL::ViewProjMode::WORLD);
    }

    g->m_meshRenderer->GetActiveMaterial()->SetAlbedoTexture(nullptr);
}

GameObject *RenderFactory::GetGameObject() const
{
    return m_gizmosGo;
}

void RenderFactory::Render(Renderer *rend)
{
    rend->OnRender( rend->GetActiveMaterial()->GetRenderPass() );
}

RenderFactory* RenderFactory::GetInstance()
{
    Scene *scene = SceneManager::GetActiveScene();
    return scene ? scene->GetGizmos() : nullptr;
}
