#include "Bang/WaterRenderer.h"

#include "Bang/MeshFactory.h"
#include "Bang/MaterialFactory.h"
#include "Bang/ShaderProgramFactory.h"

#include "Bang/GL.h"
#include "Bang/VAO.h"
#include "Bang/Mesh.h"

USING_NAMESPACE_BANG

WaterRenderer::WaterRenderer()
{
    p_planeMesh = MeshFactory::GetPlane();
    SetMaterial( MaterialFactory::GetWater().Get() );
}

WaterRenderer::~WaterRenderer()
{
}

void WaterRenderer::OnRender()
{
    Renderer::OnRender();

    GL::Render(p_planeMesh.Get()->GetVAO(),
               GL::Primitive::TRIANGLES,
               p_planeMesh.Get()->GetNumVertices());
}

void WaterRenderer::CloneInto(ICloneable *clone) const
{
    Renderer::CloneInto(clone);

    WaterRenderer *wrClone = SCAST<WaterRenderer*>(clone);
    (void) wrClone;
}

void WaterRenderer::ImportXML(const XMLNode &xmlInfo)
{
    Renderer::ImportXML(xmlInfo);
}

void WaterRenderer::ExportXML(XMLNode *xmlInfo) const
{
    Renderer::ExportXML(xmlInfo);
}

