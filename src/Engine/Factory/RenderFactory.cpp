#include "Bang/RenderFactory.h"

#include "Bang/AABox.h"
#include "Bang/Array.tcc"
#include "Bang/Assets.h"
#include "Bang/Assets.tcc"
#include "Bang/Camera.h"
#include "Bang/GBuffer.h"
#include "Bang/GEngine.h"
#include "Bang/GL.h"
#include "Bang/GameObject.h"
#include "Bang/GameObject.tcc"
#include "Bang/GameObjectFactory.h"
#include "Bang/HideFlags.h"
#include "Bang/LineRenderer.h"
#include "Bang/Material.h"
#include "Bang/MaterialFactory.h"
#include "Bang/Math.h"
#include "Bang/Mesh.h"
#include "Bang/MeshFactory.h"
#include "Bang/MeshRenderer.h"
#include "Bang/Paths.h"
#include "Bang/Quaternion.h"
#include "Bang/Rect.h"
#include "Bang/RenderPass.h"
#include "Bang/Renderer.h"
#include "Bang/ShaderProgram.h"
#include "Bang/ShaderProgramFactory.h"
#include "Bang/Texture2D.h"
#include "Bang/Transform.h"
#include "Bang/Vector3.h"

using namespace Bang;

RenderFactory::RenderFactory()
{
    m_renderGo = GameObjectFactory::CreateGameObject();
    m_renderGo->SetName("Gizmos");

    m_boxMesh = MeshFactory::GetCube();
    m_planeMesh = MeshFactory::GetUIPlane();
    m_sphereMesh = MeshFactory::GetSphere();
    m_outlineShaderProgram.Set(ShaderProgramFactory::Get(
        ShaderProgramFactory::GetScreenPassVertexShaderPath(),
        EPATH("Shaders").Append("GizmosOutline.frag")));

    m_lineRenderer = m_renderGo->AddComponent<LineRenderer>();
    m_meshRenderer = m_renderGo->AddComponent<MeshRenderer>();
    m_renderers = m_renderGo->GetComponents<Renderer>();

    for (Renderer *rend : m_renderers)
    {
        rend->SetMaterial(MaterialFactory::GetGizmosUnLightedOverlay().Get());
    }

    m_renderGo->Start();
    m_renderGo->GetHideFlags().SetOn(HideFlag::DONT_SERIALIZE);
}

RenderFactory::~RenderFactory()
{
    GameObject::Destroy(m_renderGo);
}

void RenderFactory::RenderCustomMesh(Mesh *m,
                                     const RenderFactory::Parameters &params)
{
    if (RenderFactory *rf = RenderFactory::GetInstance())
    {
        RenderFactory::Parameters paramsCpy = params;
        paramsCpy.viewProjMode = GL::ViewProjMode::WORLD;
        rf->m_meshRenderer->SetMesh(m);

        Render(rf->m_meshRenderer, paramsCpy);
    }
}

void RenderFactory::RenderWireframeBox(const AABox &b,
                                       const RenderFactory::Parameters &params)
{
    const Quaternion &r = params.rotation;
    const Vector3 bMin = b.GetMin() * params.scale;
    const Vector3 bMax = b.GetMax() * params.scale;

    RenderFactory::Parameters paramsCpy = params;
    paramsCpy.rotation = Quaternion::Identity();
    paramsCpy.scale = Vector3(1.0f);
    paramsCpy.wireframe = true;

    RenderLine(r * Vector3(bMin.x, bMin.y, bMin.z),
               r * Vector3(bMax.x, bMin.y, bMin.z),
               paramsCpy);
    RenderLine(r * Vector3(bMin.x, bMin.y, bMin.z),
               r * Vector3(bMin.x, bMax.y, bMin.z),
               paramsCpy);
    RenderLine(r * Vector3(bMin.x, bMin.y, bMin.z),
               r * Vector3(bMin.x, bMin.y, bMax.z),
               paramsCpy);

    RenderLine(r * Vector3(bMax.x, bMin.y, bMin.z),
               r * Vector3(bMax.x, bMax.y, bMin.z),
               paramsCpy);
    RenderLine(r * Vector3(bMax.x, bMin.y, bMin.z),
               r * Vector3(bMax.x, bMin.y, bMax.z),
               paramsCpy);

    RenderLine(r * Vector3(bMin.x, bMax.y, bMin.z),
               r * Vector3(bMax.x, bMax.y, bMin.z),
               paramsCpy);
    RenderLine(r * Vector3(bMin.x, bMax.y, bMin.z),
               r * Vector3(bMin.x, bMax.y, bMax.z),
               paramsCpy);

    RenderLine(r * Vector3(bMin.x, bMin.y, bMax.z),
               r * Vector3(bMax.x, bMin.y, bMax.z),
               paramsCpy);
    RenderLine(r * Vector3(bMin.x, bMin.y, bMax.z),
               r * Vector3(bMin.x, bMax.y, bMax.z),
               paramsCpy);

    RenderLine(r * Vector3(bMin.x, bMax.y, bMax.z),
               r * Vector3(bMax.x, bMax.y, bMax.z),
               paramsCpy);
    RenderLine(r * Vector3(bMax.x, bMin.y, bMax.z),
               r * Vector3(bMax.x, bMax.y, bMax.z),
               paramsCpy);
    RenderLine(r * Vector3(bMax.x, bMax.y, bMin.z),
               r * Vector3(bMax.x, bMax.y, bMax.z),
               paramsCpy);
}

void RenderFactory::RenderBox(const AABox &b,
                              const RenderFactory::Parameters &params)
{
    if (RenderFactory *rf = RenderFactory::GetInstance())
    {
        rf->m_meshRenderer->SetMesh(rf->m_boxMesh.Get());

        RenderFactory::Parameters paramsCpy = params;
        paramsCpy.viewProjMode = GL::ViewProjMode::WORLD;
        paramsCpy.position = b.GetCenter();
        paramsCpy.scale *= b.GetSize();

        rf->Render(rf->m_meshRenderer, paramsCpy);
    }
}

void RenderFactory::RenderRectNDC(const AARect &r,
                                  const RenderFactory::Parameters &params)
{
    RenderFactory::RenderRectNDC(
        {{r.GetMinXMinY(), r.GetMaxXMinY(), r.GetMaxXMaxY(), r.GetMinXMaxY()}},
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
    RenderRect(
        {{r.GetLeftBot(), r.GetRightBot(), r.GetLeftTop(), r.GetRightTop()}},
        params);
}

void RenderFactory::RenderFillRect(const AARect &r,
                                   const RenderFactory::Parameters &params)
{
    if (RenderFactory *rf = RenderFactory::GetInstance())
    {
        rf->m_meshRenderer->SetMesh(rf->m_planeMesh.Get());

        RenderFactory::Parameters paramsCpy = params;
        paramsCpy.viewProjMode = GL::ViewProjMode::CANVAS;
        paramsCpy.position = Vector3(r.GetCenter(), 0);
        paramsCpy.scale = Vector3(r.GetSize(), 1);

        rf->Render(rf->m_meshRenderer, paramsCpy);
    }
}

void RenderFactory::RenderIcon(Texture2D *texture,
                               bool billboard,
                               const RenderFactory::Parameters &params)
{
    if (RenderFactory *rf = RenderFactory::GetInstance())
    {
        rf->m_meshRenderer->SetMesh(rf->m_planeMesh.Get());

        RenderFactory::Parameters paramsCpy = params;
        paramsCpy.wireframe = false;
        paramsCpy.receivesLighting = false;
        if (billboard)
        {
            Quaternion bbRotation;
            Vector3 bbScale;
            RenderFactory::GetBillboardTransform(
                paramsCpy.position, &bbRotation, &bbScale);
            paramsCpy.rotation = bbRotation;
            paramsCpy.scale *= bbScale;
        }
        paramsCpy.texture.Set(texture);
        rf->Render(rf->m_meshRenderer, paramsCpy);
    }
}

void RenderFactory::RenderViewportIcon(Texture2D *texture,
                                       const AARect &winRect,
                                       const RenderFactory::Parameters &params)
{
    RenderFactory *rf = RenderFactory::GetInstance();
    if (!rf)
    {
        return;
    }
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
    if (RenderFactory *rf = RenderFactory::GetInstance())
    {
        RenderFactory::Parameters paramsCpy = params;
        paramsCpy.viewProjMode = GL::ViewProjMode::WORLD;

        rf->m_lineRenderer->SetPoints({origin, destiny});
        rf->Render(rf->m_lineRenderer, paramsCpy);
    }
}

void RenderFactory::RenderBillboardCircle(
    float radius,
    const RenderFactory::Parameters &params,
    int numSegments)
{
    RenderFactory *rf = RenderFactory::GetInstance();
    if (!rf)
    {
        return;
    }

    Camera *cam = Camera::GetActive();
    Vector3 camPos = cam ? cam->GetGameObject()->GetTransform()->GetPosition()
                         : Vector3::Zero();

    RenderFactory::Parameters paramsCpy = params;
    paramsCpy.rotation = Quaternion::LookDirection(
        (camPos - paramsCpy.position).NormalizedSafe());
    RenderFactory::RenderWireframeCircle(radius, paramsCpy, numSegments);
}

void RenderFactory::RenderWireframeCircle(
    float radius,
    const RenderFactory::Parameters &params,
    int numSegments,
    bool hemicircle)
{
    if (RenderFactory *rf = RenderFactory::GetInstance())
    {
        float wholeAngle = SCAST<float>(Math::Pi * (hemicircle ? 1 : 2));
        Array<Vector3> circlePoints;
        for (int i = 0; i < numSegments; ++i)
        {
            float angle = (wholeAngle / (numSegments - 1)) * i;
            Vector3 point = Vector3(
                Math::Cos(angle) * radius, Math::Sin(angle) * radius, 0.0f);
            circlePoints.PushBack(point);
        }

        int n = (hemicircle ? (numSegments - 1) : numSegments);
        Array<Vector3> circleLinePoints;
        for (int i = 0; i < n; ++i)
        {
            Vector3 p0 = circlePoints[i];
            Vector3 p1 = circlePoints[(i + 1) % numSegments];
            circleLinePoints.PushBack(p0);
            circleLinePoints.PushBack(p1);
        }

        RenderFactory::Parameters paramsCpy = params;
        paramsCpy.viewProjMode = GL::ViewProjMode::WORLD;

        rf->m_lineRenderer->SetPoints(circleLinePoints);
        rf->Render(rf->m_lineRenderer, paramsCpy);
    }
}

void RenderFactory::RenderViewportLineNDC(
    const Vector2 &origin,
    const Vector2 &destiny,
    const RenderFactory::Parameters &params)
{
    if (RenderFactory *rf = RenderFactory::GetInstance())
    {
        Vector2 originVP = GL::FromViewportPointNDCToViewportPoint(origin);
        Vector2 destinyVP = GL::FromViewportPointNDCToViewportPoint(destiny);

        RenderFactory::Parameters paramsCpy = params;
        paramsCpy.viewProjMode = GL::ViewProjMode::CANVAS;

        rf->m_lineRenderer->SetPoints({Vector3(originVP.x, originVP.y, 0),
                                       Vector3(destinyVP.x, destinyVP.y, 0)});
        rf->Render(rf->m_lineRenderer, paramsCpy);
    }
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
    if (RenderFactory *rf = RenderFactory::GetInstance())
    {
        rf->m_meshRenderer->SetMesh(rf->m_sphereMesh.Get());

        RenderFactory::Parameters paramsCpy = params;
        paramsCpy.scale = Vector3(radius);
        paramsCpy.viewProjMode = GL::ViewProjMode::WORLD;

        rf->Render(rf->m_meshRenderer, paramsCpy);
    }
}

void RenderFactory::RenderWireframeCapsule(
    float height,
    float radius,
    const RenderFactory::Parameters &params)
{
    RenderFactory::Parameters paramsCpy = params;

    const float hHeight = (height / 2);
    RenderFactory::RenderLine(
        Vector3(radius, -hHeight, 0), Vector3(radius, +hHeight, 0), paramsCpy);
    RenderFactory::RenderLine(Vector3(-radius, -hHeight, 0),
                              Vector3(-radius, +hHeight, 0),
                              paramsCpy);
    RenderFactory::RenderLine(
        Vector3(0, -hHeight, radius), Vector3(0, +hHeight, radius), paramsCpy);
    RenderFactory::RenderLine(Vector3(0, -hHeight, -radius),
                              Vector3(0, +hHeight, -radius),
                              paramsCpy);

    Vector3 capsuleCenter = paramsCpy.position;
    paramsCpy.position =
        capsuleCenter + paramsCpy.rotation * Vector3(0, hHeight, 0);
    RenderFactory::RenderWireframeSphere(
        radius, false, paramsCpy, 1, 1, 16, true);
    paramsCpy.rotation =
        params.rotation *
        Quaternion::AngleAxis(SCAST<float>(Math::Pi), Vector3::Forward());

    paramsCpy.position =
        capsuleCenter + paramsCpy.rotation * Vector3(0, hHeight, 0);
    RenderFactory::RenderWireframeSphere(
        radius, false, paramsCpy, 1, 1, 16, true);
}

void RenderFactory::RenderWireframeSphere(
    float radius,
    bool withOutline,
    const RenderFactory::Parameters &params,
    int numLoopsVertical,
    int numLoopsHorizontal,
    int numCircleSegments,
    bool hemisphere)
{
    if (RenderFactory *rf = RenderFactory::GetInstance())
    {
        RenderFactory::Parameters paramsCpy = params;
        paramsCpy.viewProjMode = GL::ViewProjMode::WORLD;

        if (numLoopsVertical > 0)
        {
            const float angleAdv = SCAST<float>((Math::Pi / numLoopsVertical) /
                                                (hemisphere ? 2 : 1));
            for (int i = 0; i < numLoopsVertical; ++i)
            {
                paramsCpy.rotation =
                    params.rotation *
                    Quaternion::AngleAxis(angleAdv * i, Vector3::Right());
                RenderFactory::RenderWireframeCircle(
                    radius, paramsCpy, numCircleSegments, hemisphere);
            }
        }

        if (numLoopsHorizontal > 0)
        {
            const float angleAdv = SCAST<float>(Math::Pi / numLoopsHorizontal);
            for (int i = 0; i < numLoopsHorizontal; ++i)
            {
                paramsCpy.rotation =
                    params.rotation *
                    Quaternion::AngleAxis(SCAST<float>(Math::Pi / 2),
                                          Vector3::Up()) *
                    Quaternion::AngleAxis(angleAdv * i, Vector3::Forward());
                RenderFactory::RenderWireframeCircle(
                    radius, paramsCpy, numCircleSegments, hemisphere);
            }
        }

        if (hemisphere)
        {
            // Render base
            paramsCpy.rotation =
                params.rotation *
                Quaternion::AngleAxis(SCAST<float>(Math::Pi * 0.5f),
                                      Vector3::Right());
            RenderFactory::RenderWireframeCircle(
                radius, paramsCpy, numCircleSegments, false);
        }

        if (withOutline)
        {
            for (Renderer *r : rf->m_renderers)
            {
                r->SetEnabled(false);
            }

            paramsCpy.scale = Vector3(radius);
            rf->m_meshRenderer->SetEnabled(true);
            rf->m_meshRenderer->SetMesh(rf->m_sphereMesh.Get());
            RenderFactory::ApplyRenderParameters(rf->m_meshRenderer, paramsCpy);
            RenderFactory::RenderOutline(rf->m_renderGo, paramsCpy);

            for (Renderer *r : rf->m_renderers)
            {
                r->SetEnabled(true);
            }
        }
    }
}

void RenderFactory::RenderOutline(GameObject *gameObject,
                                  const RenderFactory::Parameters &params,
                                  float alphaDepthOnFade)
{
    if (RenderFactory *rf = RenderFactory::GetInstance())
    {
        GL::Push(GL::Pushable::COLOR_MASK);
        GL::Push(GL::Pushable::DEPTH_STATES);
        GL::Push(GL::Pushable::STENCIL_STATES);
        GL::Push(GL::BindTarget::SHADER_PROGRAM);
        GL::Push(GL::Pushable::FRAMEBUFFER_AND_READ_DRAW_ATTACHMENTS);

        if (GBuffer *gbuffer = GEngine::GetActiveGBuffer())
        {
            gbuffer->PushDrawAttachments();
            gbuffer->PushDepthStencilTexture();
            gbuffer->Bind();
            gbuffer
                ->SetOverlayDepthStencil();  // Pick an unused gbuffer d/s tex

            // Render depth
            gbuffer->SetDrawBuffers({});
            GL::ClearDepthBuffer();
            GL::SetDepthMask(true);
            GL::SetDepthFunc(GL::Function::ALWAYS);
            GL::SetStencilOp(GL::StencilOperation::KEEP);
            GL::SetEnabled(GL::Enablable::STENCIL_TEST, false);
            GL::SetColorMask(false, false, false, false);

            // Mark the depth buffer drawing the object
            GEngine::GetInstance()->RenderWithPass(gameObject,
                                                   RenderPass::SCENE_OPAQUE);

            // Prepare masks and uniforms to actually draw the outline
            ShaderProgram *sp = rf->m_outlineShaderProgram.Get();
            sp->Bind();
            sp->SetColor("B_OutlineColor", params.color);
            sp->SetInt("B_OutlineThickness", SCAST<int>(params.thickness));
            sp->SetFloat("B_AlphaFadeOnDepth", alphaDepthOnFade);
            sp->SetTexture2D("B_OutlineDepthTexture",
                             gbuffer->GetDepthStencilTexture(),
                             false);
            sp->SetTexture2D("B_SceneDepthTexture",
                             gbuffer->GetSceneDepthStencilTexture(),
                             false);

            GL::SetDepthMask(false);
            GL::SetDepthFunc(GL::Function::ALWAYS);
            GL::SetColorMask(true, true, true, true);
            gbuffer->SetColorDrawBuffer();
            gbuffer->SetSceneDepthStencil();
            gbuffer->ApplyPassBlend(sp,
                                    GL::BlendFactor::SRC_ALPHA,
                                    GL::BlendFactor::ONE_MINUS_SRC_ALPHA);

            // Clear depth buffer to leave it as it was (maybe xD)
            GL::SetDepthMask(true);
            gbuffer->SetOverlayDepthStencil();
            GL::ClearDepthBuffer();

            gbuffer->PopDepthStencilTexture();
            gbuffer->PopDrawAttachments();
        }

        GL::Pop(GL::Pushable::FRAMEBUFFER_AND_READ_DRAW_ATTACHMENTS);
        GL::Pop(GL::BindTarget::SHADER_PROGRAM);
        GL::Pop(GL::Pushable::STENCIL_STATES);
        GL::Pop(GL::Pushable::DEPTH_STATES);
        GL::Pop(GL::Pushable::COLOR_MASK);
    }
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
    Vector3 farPlaneCenter = c + forward * zFar;

    float nearHeight2 = zNear * Math::Tan(fovH);
    float nearWidth2 = nearHeight2 * aspectRatio;
    float farHeight2 = zFar * Math::Tan(fovH);
    float farWidth2 = farHeight2 * aspectRatio;

    Vector3 nearUpLeft =
        nearPlaneCenter - right * nearWidth2 + up * nearHeight2;
    Vector3 nearUpRight =
        nearPlaneCenter + right * nearWidth2 + up * nearHeight2;
    Vector3 nearDownRight =
        nearPlaneCenter + right * nearWidth2 - up * nearHeight2;
    Vector3 nearDownLeft =
        nearPlaneCenter - right * nearWidth2 - up * nearHeight2;

    Vector3 farUpLeft = farPlaneCenter - right * farWidth2 + up * farHeight2;
    Vector3 farUpRight = farPlaneCenter + right * farWidth2 + up * farHeight2;
    Vector3 farDownRight = farPlaneCenter + right * farWidth2 - up * farHeight2;
    Vector3 farDownLeft = farPlaneCenter - right * farWidth2 - up * farHeight2;

    // Near plane
    RenderFactory::RenderLine(nearUpLeft, nearUpRight, params);
    RenderFactory::RenderLine(nearUpRight, nearDownRight, params);
    RenderFactory::RenderLine(nearDownRight, nearDownLeft, params);
    RenderFactory::RenderLine(nearDownLeft, nearUpLeft, params);

    // Far plane
    RenderFactory::RenderLine(farUpLeft, farUpRight, params);
    RenderFactory::RenderLine(farUpRight, farDownRight, params);
    RenderFactory::RenderLine(farDownRight, farDownLeft, params);
    RenderFactory::RenderLine(farDownLeft, farUpLeft, params);

    // Projection lines
    RenderFactory::RenderLine(nearUpLeft, farUpLeft, params);
    RenderFactory::RenderLine(nearUpRight, farUpRight, params);
    RenderFactory::RenderLine(nearDownRight, farDownRight, params);
    RenderFactory::RenderLine(nearDownLeft, farDownLeft, params);
}

void RenderFactory::RenderPoint(const Vector3 &point,
                                const RenderFactory::Parameters &params)
{
    if (RenderFactory *rf = RenderFactory::GetInstance())
    {
        AH<Mesh> rhm = Assets::Create<Mesh>();
        Mesh *m = rhm.Get();
        m->SetPositionsPool({point});
        m->UpdateVAOs();

        rf->m_meshRenderer->SetMesh(m);
        rf->m_meshRenderer->SetRenderPrimitive(GL::Primitive::POINTS);

        Render(rf->m_meshRenderer, params);

        rf->m_meshRenderer->SetRenderPrimitive(GL::Primitive::TRIANGLES);
    }
}

void RenderFactory::RenderPointNDC(const Vector2 &pointNDC,
                                   const RenderFactory::Parameters &params_)
{
    if (RenderFactory *rf = RenderFactory::GetInstance())
    {
        GL::Push(GL::Pushable::VIEWPROJ_MODE);

        Vector2i pointVP(GL::FromViewportPointNDCToViewportPoint(pointNDC));
        AH<Mesh> rhm = Assets::Create<Mesh>();
        Mesh *m = rhm.Get();
        m->SetPositionsPool({Vector3(pointVP, 0)});
        m->UpdateVAOs();

        rf->m_meshRenderer->SetMesh(m);
        rf->m_meshRenderer->SetRenderPrimitive(GL::Primitive::POINTS);

        RenderFactory::Parameters params = params_;
        params.cullFace = GL::CullFaceExt::NONE;
        params.viewProjMode = GL::ViewProjMode::CANVAS;
        Render(rf->m_meshRenderer, params);

        rf->m_meshRenderer->SetRenderPrimitive(GL::Primitive::TRIANGLES);

        GL::Pop(GL::Pushable::VIEWPROJ_MODE);
    }
}

void RenderFactory::GetBillboardTransform(const Vector3 &center,
                                          Quaternion *rotation,
                                          Vector3 *scale)
{
    Camera *cam = Camera::GetActive();

    Vector3 camPos = cam->GetGameObject()->GetTransform()->GetPosition();
    float distScale = 1.0f;
    if (cam->GetProjectionMode() == CameraProjectionMode::PERSPECTIVE)
    {
        distScale = Vector3::Distance(camPos, center);
    }

    *scale = Vector3(distScale * 0.5f);
    Transform *camTR = cam->GetGameObject()->GetTransform();
    *rotation = Quaternion::LookDirection(camTR->GetForward(), camTR->GetUp());
}

GameObject *RenderFactory::GetGameObject() const
{
    return m_renderGo;
}

void RenderFactory::Render(Renderer *rend,
                           const RenderFactory::Parameters &params)
{
    ApplyRenderParameters(rend, params);
    rend->OnRender(rend->GetActiveMaterial()
                       ->GetShaderProgramProperties()
                       .GetRenderPass());
}

RenderFactory *RenderFactory::GetInstance()
{
    if (GEngine *ge = GEngine::GetInstance())
    {
        return ge->GetRenderFactory();
    }
    return nullptr;
}

void RenderFactory::ApplyRenderParameters(
    Renderer *rend,
    const RenderFactory::Parameters &params)
{
    if (rend)
    {
        Material *mat = rend->GetMaterial();
        mat->SetAlbedoColor(params.color);
        mat->SetReceivesLighting(params.receivesLighting);
        mat->GetShaderProgramProperties().SetWireframe(params.wireframe);
        mat->GetShaderProgramProperties().SetCullFace(params.cullFace);
        mat->GetShaderProgramProperties().SetLineWidth(params.thickness);
        mat->SetAlbedoTexture(params.texture.Get());

        rend->SetDepthMask(params.depthMask);
        rend->SetViewProjMode(params.viewProjMode);
    }

    if (RenderFactory *rf = RenderFactory::GetInstance())
    {
        rf->GetGameObject()->GetTransform()->SetPosition(params.position);
        rf->GetGameObject()->GetTransform()->SetRotation(params.rotation);
        rf->GetGameObject()->GetTransform()->SetLocalScale(params.scale);
    }
}
