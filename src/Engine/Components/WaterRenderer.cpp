#include "Bang/WaterRenderer.h"

#include "Bang/ClassDB.h"
#include "Bang/GL.h"
#include "Bang/Material.h"
#include "Bang/MaterialFactory.h"
#include "Bang/Mesh.h"
#include "Bang/MeshFactory.h"

namespace Bang
{
class ICloneable;
class MetaNode;
}

using namespace Bang;

WaterRenderer::WaterRenderer()
{
    SET_INSTANCE_CLASS_ID(WaterRenderer);

    p_planeMesh = MeshFactory::GetPlane();
    SetMaterial(MaterialFactory::GetWater().Get());

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

void WaterRenderer::Reflect()
{
    Renderer::Reflect();
}
