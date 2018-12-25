#ifndef DEBUGRENDERER_H
#define DEBUGRENDERER_H

#include "Bang/Array.h"
#include "Bang/AssetHandle.h"
#include "Bang/BangDefines.h"
#include "Bang/Color.h"
#include "Bang/GL.h"
#include "Bang/GameObject.h"
#include "Bang/List.h"
#include "Bang/String.h"
#include "Bang/Time.h"

namespace Bang
{
class AABox;
class Mesh;
class Quad;
class Triangle;

class DebugRenderer : public GameObject
{
    GAMEOBJECT(DebugRenderer);

public:
    DebugRenderer();

    static void Clear();

    static void RenderLine(const Vector3 &origin,
                           const Vector3 &end,
                           const Color &color = Color::Green(),
                           float time = 1.0f,
                           float thickness = 1.0f,
                           bool depthTest = false);

    static void RenderPoint(const Vector3 &point,
                            const Color &color = Color::Green(),
                            float time = 1.0f,
                            float thickness = 1.0f,
                            bool depthTest = false);

    static void RenderPointNDC(const Vector2 &point,
                               const Color &color = Color::Green(),
                               float time = 1.0f,
                               float thickness = 1.0f);

    static void RenderLineNDC(const Vector2 &originNDC,
                              const Vector2 &endNDC,
                              const Color &color = Color::Green(),
                              float time = 1.0f,
                              float thickness = 1.0f,
                              bool depthTest = false);

    static void RenderAABox(const AABox &aaBox,
                            const Color &color = Color::Green(),
                            float time = 1.0f,
                            float thickness = 1.0f,
                            bool wireframe = false,
                            bool culling = false,
                            bool depthTest = false,
                            const Color &bordersColor = Color::Zero());

    static void RenderTriangle(const Triangle &triangle,
                               const Color &color = Color::Green(),
                               float time = 1.0f,
                               bool wireframe = false,
                               bool culling = false,
                               bool depthTest = false);
    static void RenderQuad(const Quad &quad,
                           const Color &color = Color::Green(),
                           float time = 1.0f,
                           bool wireframe = false,
                           bool culling = false,
                           bool depthTest = false,
                           const Color &bordersColor = Color::Zero());

    static void RenderAARectNDC(const AARect &aaRectNDC,
                                const Color &color = Color::Green(),
                                float time = 1.0f,
                                float thickness = 1.0f,
                                bool depthTest = false);

    static void RenderRectNDC(const Rect &rectNDC,
                              const Color &color = Color::Green(),
                              float time = 1.0f,
                              float thickness = 1.0f,
                              bool depthTest = false);

private:
    enum class DebugRendererPrimitiveType
    {
        POINT,
        POINT_NDC,
        LINE,
        LINE_NDC,
        AARECT_NDC,
        RECT_NDC,
        TRIANGLE,
        QUAD
    };

    struct DebugRenderPrimitive
    {
        DebugRendererPrimitiveType primitive;
        Vector3 p0, p1, p2, p3;
        RectPoints rectNDCPoints;
        AARect aaRectNDC;
        Color color;
        float thickness;
        Time destroyTime;
        bool wireframe;
        GL::CullFaceExt cullFace;
        bool depthTest;
        bool renderedOnce;
    };

    AH<Mesh> m_mesh;
    List<DebugRenderPrimitive> m_primitivesToRender;

    virtual ~DebugRenderer() override;

    void RenderPrimitives(bool withDepth);

    static DebugRenderPrimitive *CreateDebugRenderPrimitive(
        DebugRendererPrimitiveType primitive,
        const Array<Vector3> &points,
        const Color &color,
        float time,
        float thickness,
        bool wireframe,
        bool culling,
        bool depthTest);

    static DebugRenderer *GetActive();

    friend class Scene;
};
}

#endif  // DEBUGRENDERER_H
