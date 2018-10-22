#include "Bang/MeshRenderer.h"

#include "Bang/Extensions.h"
#include "Bang/FastDynamicCast.h"
#include "Bang/GL.h"
#include "Bang/GUID.h"
#include "Bang/ICloneable.h"
#include "Bang/Material.h"
#include "Bang/MaterialFactory.h"
#include "Bang/Math.h"
#include "Bang/Mesh.h"
#include "Bang/MeshFactory.h"
#include "Bang/MetaNode.h"
#include "Bang/MetaNode.tcc"
#include "Bang/ReflectionProbe.h"
#include "Bang/Resources.h"
#include "Bang/Resources.tcc"
#include "Bang/ShaderProgram.h"
#include "Bang/TypeTraits.h"

using namespace Bang;

MeshRenderer::MeshRenderer()
{
    CONSTRUCT_CLASS_ID(MeshRenderer);
    SetRenderPrimitive(GL::Primitive::TRIANGLES);
    SetMaterial(MaterialFactory::GetDefault().Get());
    SetMesh(MeshFactory::GetCube().Get());
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

Mesh *MeshRenderer::GetMesh() const
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
    int maxLOD = GetActiveMesh() ? GetActiveMesh()->GetNumLODs() - 1 : 0;
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
    return GetActiveMesh() ? GetActiveMesh()->GetLODMesh(GetCurrentLOD()).Get()
                           : nullptr;
}

AABox MeshRenderer::GetAABBox() const
{
    return GetActiveMesh() ? GetActiveMesh()->GetAABBox() : AABox::Empty;
}

void MeshRenderer::OnRender()
{
    Renderer::OnRender();

    if (Mesh *baseMeshToRender = GetActiveMesh())
    {
        Mesh *lodMeshToRender =
            baseMeshToRender->GetLODMesh(GetCurrentLOD()).Get();
        GL::Render(lodMeshToRender->GetVAO(),
                   GetRenderPrimitive(),
                   lodMeshToRender->GetNumVerticesIds());
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

void MeshRenderer::Reflect()
{
    Renderer::Reflect();

    BANG_REFLECT_VAR_MEMBER_RESOURCE(
        MeshRenderer,
        "Mesh",
        SetMesh,
        GetSharedMesh,
        Mesh,
        BANG_REFLECT_HINT_EXTENSIONS(Extensions::GetMeshExtension()) +
            BANG_REFLECT_HINT_ZOOMABLE_PREVIEW(true));
}
