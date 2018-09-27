#include "Bang/MeshRenderer.h"

#include "Bang/GL.h"
#include "Bang/VAO.h"
#include "Bang/Mesh.h"
#include "Bang/ModelIO.h"
#include "Bang/Material.h"
#include "Bang/MetaNode.h"
#include "Bang/Transform.h"
#include "Bang/Resources.h"
#include "Bang/GameObject.h"
#include "Bang/MeshFactory.h"
#include "Bang/ShaderProgram.h"
#include "Bang/MaterialFactory.h"
#include "Bang/ReflectionProbe.h"

USING_NAMESPACE_BANG

MeshRenderer::MeshRenderer()
{
    CONSTRUCT_CLASS_ID(MeshRenderer);
    SetRenderPrimitive( GL::Primitive::TRIANGLES );
    SetMaterial( MaterialFactory::GetDefault().Get() );
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

Mesh *MeshRenderer::GetSharedMesh() const
{
    return p_sharedMesh.Get();
}

void MeshRenderer::Bind()
{
    Renderer::Bind();
}

void MeshRenderer::SetUniformsOnBind(ShaderProgram *sp)
{
    Renderer::SetUniformsOnBind(sp);

    sp->SetBool("B_HasBoneAnimations", false);
    ReflectionProbe::SetRendererUniforms(this, sp);
}

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
        Mesh *lodMeshToRender =
                baseMeshToRender->GetLODMesh( GetCurrentLOD() ).Get();
        GL::Render(lodMeshToRender->GetVAO(),
                   GetRenderPrimitive(),
                   lodMeshToRender->GetNumVertices());
    }
}

Mesh *MeshRenderer::GetActiveMesh() const
{
    if (p_mesh)
    {
        return GetMesh();
    }
    return GetSharedMesh();
}

void MeshRenderer::CloneInto(ICloneable *clone) const
{
    Renderer::CloneInto(clone);
    MeshRenderer *mr = Cast<MeshRenderer*>(clone);
    mr->SetMesh( GetSharedMesh() );
}

void MeshRenderer::ImportMeta(const MetaNode &metaNode)
{
    Renderer::ImportMeta(metaNode);
    if (metaNode.Contains("Mesh"))
    {
        RH<Mesh> mesh = Resources::Load<Mesh>(metaNode.Get<GUID>("Mesh"));
        SetMesh( mesh.Get() );
    }
}

void MeshRenderer::ExportMeta(MetaNode *metaNode) const
{
    Renderer::ExportMeta(metaNode);
    metaNode->Set("Mesh", GetSharedMesh() ? GetSharedMesh()->GetGUID() :
                                           GUID::Empty());
}
