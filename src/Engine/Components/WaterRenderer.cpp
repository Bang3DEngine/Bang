#include "Bang/WaterRenderer.h"

#include "Bang/FastDynamicCast.h"
#include "Bang/GL.h"
#include "Bang/MaterialFactory.h"
#include "Bang/Mesh.h"
#include "Bang/Material.h"
#include "Bang/MeshFactory.h"

FORWARD NAMESPACE_BANG_BEGIN
FORWARD class ICloneable;
FORWARD class MetaNode;
FORWARD NAMESPACE_BANG_END

USING_NAMESPACE_BANG

WaterRenderer::WaterRenderer()
{
    CONSTRUCT_CLASS_ID(WaterRenderer);

    p_planeMesh = MeshFactory::GetPlane();
    SetMaterial( MaterialFactory::GetWater().Get() );

    SetUseReflectionProbes(true);
}

WaterRenderer::~WaterRenderer()
{
}

void WaterRenderer::OnRender()
{
    Renderer::OnRender();

    GL::Render(p_planeMesh.Get()->GetVAO(),
               GL::Primitive::TRIANGLES,
               p_planeMesh.Get()->GetNumVerticesIds());
}

void WaterRenderer::CloneInto(ICloneable *clone) const
{
    Renderer::CloneInto(clone);

    WaterRenderer *wrClone = SCAST<WaterRenderer*>(clone);
    BANG_UNUSED(wrClone);
}

void WaterRenderer::ImportMeta(const MetaNode &metaNode)
{
    Renderer::ImportMeta(metaNode);
}

void WaterRenderer::ExportMeta(MetaNode *metaNode) const
{
    Renderer::ExportMeta(metaNode);
}

