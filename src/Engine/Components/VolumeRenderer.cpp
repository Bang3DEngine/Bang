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

void VolumeRenderer::SetModelPath(const Path &modelPath)
{
    m_modelPath = modelPath;
}

const Path &VolumeRenderer::GetModelPath() const
{
    return m_modelPath;
}

void VolumeRenderer::OnRender()
{
    Renderer::OnRender();

    GL::Render(GetCubeMesh()->GetVAO(),
               GL::Primitive::TRIANGLES,
               GetCubeMesh()->GetNumVerticesIds());
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
