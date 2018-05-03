#include "Bang/DebugRenderer.h"

#include "Bang/GL.h"
#include "Bang/Mesh.h"
#include "Bang/Quad.h"
#include "Bang/Debug.h"
#include "Bang/AABox.h"
#include "Bang/Scene.h"
#include "Bang/Gizmos.h"
#include "Bang/Triangle.h"
#include "Bang/Resources.h"
#include "Bang/SceneManager.h"

USING_NAMESPACE_BANG

DebugRenderer::DebugRenderer()
{
    m_mesh = Resources::Create<Mesh>();
}

DebugRenderer::~DebugRenderer()
{
}

void DebugRenderer::Clear()
{
    DebugRenderer *dr = DebugRenderer::GetActive(); ASSERT(dr);
    dr->m_primitivesToRender.Clear();
}

void DebugRenderer::RenderLine(const Vector3 &origin,
                               const Vector3 &end,
                               const Color &color,
                               float time,
                               float thickness,
                               bool depthTest)
{
    CreateDebugRenderPrimitive(DebugRendererPrimitiveType::Line,
                               {origin, end},
                               color, time, thickness, false, false, depthTest);
}

void DebugRenderer::RenderPoint(const Vector3 &point,
                                const Color &color,
                                float time,
                                float thickness,
                                bool depthTest)
{
    CreateDebugRenderPrimitive(DebugRendererPrimitiveType::Point,
                               {point},
                               color, time, thickness, false, false, depthTest);
}

void DebugRenderer::RenderLineNDC(const Vector2 &originNDC,
                                  const Vector2 &endNDC,
                                  const Color &color,
                                  float time,
                                  float thickness,
                                  bool depthTest)
{
    CreateDebugRenderPrimitive(DebugRendererPrimitiveType::LineNDC,
                               {Vector3(originNDC,0), Vector3(endNDC,0)},
                               color, time, thickness, false, false, depthTest);
}

void DebugRenderer::RenderAABox(const AABox &aaBox,
                                const Color &color,
                                float time,
                                float thickness,
                                bool culling,
                                bool wireframe,
                                bool depthTest,
                                const Color &bordersColor)
{
    DebugRenderer::RenderQuad(aaBox.GetLeftQuad(), color, time,
                              wireframe, culling, depthTest, bordersColor);
    DebugRenderer::RenderQuad(aaBox.GetRightQuad(), color, time,
                              wireframe, culling, depthTest, bordersColor);
    DebugRenderer::RenderQuad(aaBox.GetTopQuad(), color, time,
                              wireframe, culling, depthTest, bordersColor);
    DebugRenderer::RenderQuad(aaBox.GetBotQuad(), color, time,
                              wireframe, culling, depthTest, bordersColor);
    DebugRenderer::RenderQuad(aaBox.GetBackQuad(), color, time,
                              wireframe, culling, depthTest, bordersColor);
    DebugRenderer::RenderQuad(aaBox.GetFrontQuad(), color, time,
                              wireframe, culling, depthTest, bordersColor);
}

void DebugRenderer::RenderTriangle(const Triangle &triangle,
                                   const Color &color,
                                   float time,
                                   bool wireframe,
                                   bool culling,
                                   bool depthTest)
{
    CreateDebugRenderPrimitive(DebugRendererPrimitiveType::Triangle,
                               {triangle[0], triangle[1], triangle[2]},
                               color, time, -1.0f, wireframe, culling, depthTest);
}

void DebugRenderer::RenderQuad(const Quad &quad,
                               const Color &color,
                               float time,
                               bool wireframe,
                               bool culling,
                               bool depthTest,
                               const Color &bordersColor)
{
    CreateDebugRenderPrimitive(DebugRendererPrimitiveType::Quad,
                               {quad[0], quad[1], quad[2], quad[3]},
                               color, time, -1.0f, wireframe, culling, depthTest);

    if (bordersColor != Color::Zero)
    {
        RenderLine(quad[0], quad[1], bordersColor, time, 1.0f, depthTest);
        RenderLine(quad[1], quad[2], bordersColor, time, 1.0f, depthTest);
        RenderLine(quad[2], quad[3], bordersColor, time, 1.0f, depthTest);
        RenderLine(quad[3], quad[0], bordersColor, time, 1.0f, depthTest);
    }
}

void DebugRenderer::RenderAARectNDC(const AARect &rectNDC, const Color &color,
                                    float time, float thickness, bool depthTest)
{
    DebugRenderer::DebugRenderPrimitive *drp =
        CreateDebugRenderPrimitive(DebugRendererPrimitiveType::AARectNDC,
                                   {}, color, time, thickness, false, false, depthTest);
    if (drp)
    {
        drp->aaRectNDC = GL::FromViewportRectToViewportRectNDC(rectNDC);
    }
}

void DebugRenderer::RenderRectNDC(const Rect &rectNDC, const Color &color,
                                  float time, float thickness, bool depthTest)
{
    DebugRenderer::DebugRenderPrimitive *drp =
        CreateDebugRenderPrimitive(DebugRendererPrimitiveType::RectNDC,
                                   {}, color, time, thickness, false, false, depthTest);
    if (drp)
    {
        drp->rectNDCPoints = rectNDC.GetPoints();
    }
}

DebugRenderer::DebugRenderPrimitive*
DebugRenderer::CreateDebugRenderPrimitive(DebugRendererPrimitiveType primitive,
                                          const Array<Vector3> &points,
                                          const Color &color,
                                          float time,
                                          float thickness,
                                          bool wireframe,
                                          bool culling,
                                          bool depthTest)
{
    DebugRenderer *dr = DebugRenderer::GetActive();
    if (!dr) { Debug_Error("No active DebugRenderer!"); return nullptr; }

    DebugRenderPrimitive drp;
    drp.primitive = primitive;
    drp.p0 = (points.Size() >= 1 ? points[0] : Vector3::Zero);
    drp.p1 = (points.Size() >= 2 ? points[1] : Vector3::Zero);
    drp.p2 = (points.Size() >= 3 ? points[2] : Vector3::Zero);
    drp.p3 = (points.Size() >= 4 ? points[3] : Vector3::Zero);
    drp.color = color;
    drp.destroyTimestamp = Time::GetNow_Seconds() + time;
    drp.thickness = thickness;
    drp.wireframe = wireframe;
    drp.cullFace = (culling ? GL::CullFaceExt::Back : GL::CullFaceExt::None);
    drp.depthTest = depthTest;
    drp.renderedOnce = false;

    dr->m_primitivesToRender.PushBack(drp);

    return &(dr->m_primitivesToRender.Back());
}

void DebugRenderer::RenderPrimitives(bool withDepth)
{
    GL::Function prevDepthFunc = GL::GetDepthFunc();
    for (auto it = m_primitivesToRender.Begin(); it != m_primitivesToRender.End(); )
    {
        DebugRenderPrimitive *drp = &(*it);
        if (drp->depthTest != withDepth)
        {
            ++it;
        }
        else if (Time::GetNow_Seconds() >= drp->destroyTimestamp &&
                 drp->renderedOnce)
        {
            it = m_primitivesToRender.Remove(it);
        }
        else
        {
            Gizmos::Reset();
            GL::SetDepthFunc(drp->depthTest ? GL::Function::LEqual :
                                              GL::Function::Always);
            Gizmos::SetColor(drp->color);
            if (drp->thickness >= 0.0f) { Gizmos::SetThickness(drp->thickness); }
            switch (drp->primitive)
            {
                case DebugRendererPrimitiveType::Line:
                    Gizmos::RenderLine(drp->p0, drp->p1);
                break;

                case DebugRendererPrimitiveType::LineNDC:
                    Gizmos::RenderViewportLineNDC(drp->p0.xy(), drp->p1.xy());
                break;

                case DebugRendererPrimitiveType::Point:
                    Gizmos::RenderPoint(drp->p0);
                break;

                case DebugRendererPrimitiveType::Triangle:
                case DebugRendererPrimitiveType::Quad:
                {
                    // Load mesh
                    Array<Vector3> positions, normals;
                    Array<Vector2> uvs;
                    if (drp->primitive == DebugRendererPrimitiveType::Triangle)
                    {
                        Triangle tri(drp->p0, drp->p1, drp->p2);
                        for (int i : {0,1,2})
                        {
                            positions.PushBack(tri[i]);
                            normals.PushBack(tri.GetNormal());
                            uvs.PushBack(Vector2(0,0));
                        }
                    }
                    else if (drp->primitive == DebugRendererPrimitiveType::Quad)
                    {
                        Quad quad(drp->p0, drp->p1, drp->p2, drp->p3);
                        for (int i : {0,1,2,3,4,5})
                        {
                            (void)(i);
                            normals.PushBack(quad.GetNormal());
                            uvs.PushBack(Vector2(0,0));
                        }
                        Triangle t0, t1; quad.GetTriangles(&t0, &t1);
                        for (int i : {0,1,2}) { positions.PushBack(t0[i]); }
                        for (int i : {0,1,2}) { positions.PushBack(t1[i]); }
                    }
                    m_mesh.Get()->SetPositionsPool(positions);
                    m_mesh.Get()->SetNormalsPool(normals);
                    m_mesh.Get()->SetUvsPool(uvs);
                    m_mesh.Get()->UpdateGeometry();

                    Gizmos::SetCullFace(drp->cullFace);
                    Gizmos::SetRenderWireframe(drp->wireframe);
                    Gizmos::RenderCustomMesh( m_mesh.Get() );
                }
                break;

                case DebugRendererPrimitiveType::AARectNDC:
                    Gizmos::RenderRectNDC(
                         GL::FromViewportRectNDCToViewportRect(drp->aaRectNDC) );
                break;

                case DebugRendererPrimitiveType::RectNDC:
                    Gizmos::RenderRectNDC(drp->rectNDCPoints);
                break;

                default: ASSERT(false); break;
            }
            drp->renderedOnce = true;
            ++it;
        }
    }
    GL::SetDepthFunc(prevDepthFunc);
}

DebugRenderer *DebugRenderer::GetActive()
{
    Scene *scene = SceneManager::GetActiveScene();
    return scene ? scene->p_debugRenderer : nullptr;
}

