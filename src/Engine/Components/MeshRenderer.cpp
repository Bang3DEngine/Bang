#include "Bang/MeshRenderer.h"

#include "Bang/GL.h"
#include "Bang/VAO.h"
#include "Bang/Mesh.h"
#include "Bang/ModelIO.h"
#include "Bang/Material.h"
#include "Bang/Transform.h"
#include "Bang/Resources.h"
#include "Bang/GameObject.h"
#include "Bang/MeshFactory.h"

USING_NAMESPACE_BANG

MeshRenderer::MeshRenderer()
{
    SetRenderPrimitive( GL::Primitive::TRIANGLES );
    SetMesh( MeshFactory::GetCube().Get() );
}

MeshRenderer::~MeshRenderer()
{
}

void MeshRenderer::SetMesh(Mesh *m)
{
    if (GetSharedMesh() != m)
    {
        p_sharedMesh.Set(m);
        p_mesh.Set(nullptr);
    }
}
Mesh* MeshRenderer::GetMesh() const
{
    if (!p_mesh)
    {
        if (GetSharedMesh())
        {
            p_mesh = Resources::Clone<Mesh>(GetSharedMesh());
        }
    }
    return p_mesh.Get();
}
Mesh *MeshRenderer::GetSharedMesh() const { return p_sharedMesh.Get(); }

void MeshRenderer::SetCurrentLOD(int lod)
{
    int maxLOD = GetActiveMesh() ? GetActiveMesh()->GetNumLODs()-1 : 0;
    m_currentLOD = Math::Clamp(lod, 0, maxLOD);
}

void MeshRenderer::SetAutoLOD(bool autoLOD)
{
    m_autoLOD = autoLOD;
}

bool MeshRenderer::GetAutoLOD() const
{
    return m_autoLOD;
}

int MeshRenderer::GetCurrentLOD() const
{
    return m_currentLOD;
}

Mesh *MeshRenderer::GetCurrentLODActiveMesh() const
{
    return GetActiveMesh() ?
                GetActiveMesh()->GetLODMesh( GetCurrentLOD() ).Get() : nullptr;
}

AABox MeshRenderer::GetAABBox() const
{
    return GetActiveMesh() ? GetActiveMesh()->GetAABBox() : AABox::Empty;
}

void MeshRenderer::OnRender()
{
    Renderer::OnRender();

    Mesh *baseMeshToRender = GetActiveMesh();
    if (baseMeshToRender)
    {
        Mesh *lodMeshToRender = baseMeshToRender->GetLODMesh( GetCurrentLOD() ).Get();
        GL::Render(lodMeshToRender->GetVAO(),
                   GetRenderPrimitive(),
                   lodMeshToRender->GetNumVertices());
    }
}

Mesh *MeshRenderer::GetActiveMesh() const
{
    if (p_mesh) { return GetMesh(); }
    return GetSharedMesh();
}

void MeshRenderer::CloneInto(ICloneable *clone) const
{
    Renderer::CloneInto(clone);
    MeshRenderer *mr = Cast<MeshRenderer*>(clone);
    mr->SetMesh( GetSharedMesh() );
}

void MeshRenderer::ImportXML(const XMLNode &xmlInfo)
{
    Renderer::ImportXML(xmlInfo);
    if (xmlInfo.Contains("Mesh"))
    {
        RH<Mesh> mesh = Resources::Load<Mesh>(xmlInfo.Get<GUID>("Mesh"));
        SetMesh( mesh.Get() );
    }
}

void MeshRenderer::ExportXML(XMLNode *xmlInfo) const
{
    Renderer::ExportXML(xmlInfo);
    xmlInfo->Set("Mesh", GetSharedMesh() ? GetSharedMesh()->GetGUID() : GUID::Empty());
}
