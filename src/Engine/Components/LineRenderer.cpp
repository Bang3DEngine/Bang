#include "Bang/LineRenderer.h"

#include "Bang/AABox.h"
#include "Bang/Array.tcc"
#include "Bang/Assert.h"
#include "Bang/Assets.h"
#include "Bang/Assets.tcc"
#include "Bang/ClassDB.h"
#include "Bang/GL.h"
#include "Bang/ICloneable.h"
#include "Bang/Material.h"
#include "Bang/MaterialFactory.h"
#include "Bang/Mesh.h"
#include "Bang/MetaNode.h"
#include "Bang/ShaderProgram.h"

using namespace Bang;

LineRenderer::LineRenderer()
{
    SET_INSTANCE_CLASS_ID(LineRenderer);

    p_mesh = Assets::Create<Mesh>();

    SetCastsShadows(false);
    SetReceivesShadows(false);
    SetMaterial(MaterialFactory::GetDefault().Get());
    GetMaterial()->SetReceivesLighting(false);
    SetRenderPrimitive(GL::Primitive::LINES);
}

LineRenderer::~LineRenderer()
{
}

void LineRenderer::OnRender()
{
    Renderer::OnRender();

    if (p_mesh.Get()->GetNumVertices() > 0)
    {
        GL::Render(p_mesh.Get()->GetVAO(),
                   GetRenderPrimitive(),
                   p_mesh.Get()->GetNumVertices());
    }
}

void LineRenderer::SetPoint(int i, const Vector3 &point)
{
    Array<Vector3> newPoints = m_points;
    ASSERT(i >= 0 && i <= SCAST<int>(newPoints.Size()));

    if (i == newPoints.Size())
    {
        newPoints.PushBack(point);
    }
    else
    {
        newPoints[i] = point;
    }

    SetPoints(newPoints);
}

void LineRenderer::SetPoints(const Array<Vector3> &points)
{
    m_points = points;
    p_mesh.Get()->SetTrianglesVertexIds({});
    p_mesh.Get()->SetPositionsPool(GetPoints());
    p_mesh.Get()->UpdateVAOs();
}

const Array<Vector3> &LineRenderer::GetPoints() const
{
    return m_points;
}

AABox LineRenderer::GetAABBox() const
{
    if (m_points.IsEmpty())
    {
        return AABox::Empty();
    }

    Vector3 minp = m_points.Front();
    Vector3 maxp = m_points.Front();
    for (const Vector3 &p : m_points)
    {
        minp = Vector3::Min(minp, p);
        maxp = Vector3::Max(maxp, p);
    }

    // Add a bit in every dimensions, to avoid flattened Rects
    const float strokeAdd = 0.01f;
    minp -= Vector3(strokeAdd);
    maxp += Vector3(strokeAdd);
    return AABox(minp, maxp);
}

void LineRenderer::Bind()
{
    Renderer::Bind();
    if (Material *mat = GetActiveMaterial())
    {
        if (ShaderProgram *sp = mat->GetShaderProgram())
        {
            sp->Bind();
            sp->SetBool("B_HasBoneAnimations", false);
        }
    }
}

void LineRenderer::CloneInto(ICloneable *clone, bool cloneGUID) const
{
    Renderer::CloneInto(clone, cloneGUID);
    LineRenderer *lr = SCAST<LineRenderer *>(clone);
    lr->SetPoints(GetPoints());
}

void LineRenderer::ImportMeta(const MetaNode &metaNode)
{
    Renderer::ImportMeta(metaNode);
}

void LineRenderer::ExportMeta(MetaNode *metaNode) const
{
    Renderer::ExportMeta(metaNode);
}
