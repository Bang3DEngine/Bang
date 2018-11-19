#include "Bang/VolumeRenderer.h"

#include "Bang/GL.h"
#include "Bang/Material.h"
#include "Bang/MaterialFactory.h"
#include "Bang/Mesh.h"
#include "Bang/MeshFactory.h"
#include "Bang/Paths.h"
#include "Bang/Resources.h"
#include "Bang/ShaderProgram.h"
#include "Bang/ShaderProgramFactory.h"
#include "Bang/Texture3D.h"
#include "Bang/VAO.h"

using namespace Bang;

VolumeRenderer::VolumeRenderer()
{
    p_cubeMesh = MeshFactory::GetCube();
    p_cubeShaderProgram.Set(ShaderProgramFactory::Get(
        Paths::GetEngineAssetsDir().Append("Shaders").Append(
            "VolumeRendererCube.vert"),
        Paths::GetEngineAssetsDir().Append("Shaders").Append(
            "VolumeRendererCube.frag")));

    GetMaterial()->SetShaderProgram(p_cubeShaderProgram.Get());
}

VolumeRenderer::~VolumeRenderer()
{
}

void VolumeRenderer::SetVolumeTexture(Texture3D *volTexture)
{
    p_volumeTexture.Set(volTexture);
}

void VolumeRenderer::SetModelPath(const Path &modelPath)
{
    if (modelPath != GetModelPath())
    {
        m_modelPath = modelPath;
        p_volumeTexture = Resources::Load<Texture3D>(modelPath);
    }
}

const Path &VolumeRenderer::GetModelPath() const
{
    return m_modelPath;
}

Texture3D *VolumeRenderer::GetVolumeTexture() const
{
    return p_volumeTexture.Get();
}

void VolumeRenderer::OnRender()
{
    Renderer::OnRender();

    GL::Render(GetCubeMesh()->GetVAO(),
               GL::Primitive::TRIANGLES,
               GetCubeMesh()->GetNumVerticesIds());
}

void VolumeRenderer::SetUniformsOnBind(ShaderProgram *sp)
{
    Renderer::SetUniformsOnBind(sp);

    sp->SetTexture3D("B_Texture3D", GetVolumeTexture(), false);
}

void VolumeRenderer::Reflect()
{
    Renderer::Reflect();

    ReflectVarMember<VolumeRenderer, Path>(
        "PVM Path",
        &VolumeRenderer::SetModelPath,
        &VolumeRenderer::GetModelPath,
        this,
        BANG_REFLECT_HINT_EXTENSIONS(Array<String>({"pvm"})));
}

Mesh *VolumeRenderer::GetCubeMesh() const
{
    return p_cubeMesh.Get();
}
