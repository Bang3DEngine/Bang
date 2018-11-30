#include "Bang/VolumeRenderer.h"

#include "Bang/Extensions.h"
#include "Bang/Framebuffer.h"
#include "Bang/GBuffer.h"
#include "Bang/GEngine.h"
#include "Bang/GL.h"
#include "Bang/GLUniforms.h"
#include "Bang/Material.h"
#include "Bang/MaterialFactory.h"
#include "Bang/Mesh.h"
#include "Bang/MeshFactory.h"
#include "Bang/Paths.h"
#include "Bang/Resources.h"
#include "Bang/Shader.h"
#include "Bang/ShaderProgram.h"
#include "Bang/ShaderProgramFactory.h"
#include "Bang/Texture2D.h"
#include "Bang/Texture3D.h"
#include "Bang/TextureFactory.h"
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

    m_volumeRenderingMaterial = Resources::Create<Material>();
    m_volumeRenderingMaterial.Get()->GetNeededUniforms().SetOn(
                                    NeededUniformFlag::ALL);
    GetVolumeRenderMaterial()->SetShaderProgram(p_cubeShaderProgram.Get());

    m_cubeBackFacesGBuffer = new GBuffer(1, 1);
}

VolumeRenderer::~VolumeRenderer()
{
    delete m_cubeBackFacesGBuffer;
}

void VolumeRenderer::SetVolumeTexture(Texture3D *volTexture)
{
    p_volumeTexture.Set(volTexture);
    if (GetVolumeTexture())
    {
        GL::Push(GL::BindTarget::TEXTURE_3D);

        GetVolumeTexture()->Bind();
        GetVolumeTexture()->GenerateMipMaps();
        GetVolumeTexture()->SetFilterMode(GL::FilterMode::BILINEAR);
        GetVolumeTexture()->SetWrapMode(GL::WrapMode::CLAMP_TO_EDGE);

        GL::Pop(GL::BindTarget::TEXTURE_3D);
    }
}

void VolumeRenderer::SetModelPath(const Path &modelPath)
{
    if (modelPath != GetModelPath())
    {
        m_modelPath = modelPath;
        RH<Texture3D> tex3D = Resources::Load<Texture3D>(modelPath);
        SetVolumeTexture(tex3D.Get());
    }
}

void VolumeRenderer::SetDensityThreshold(float densityThreshold)
{
    m_densityThreshold = densityThreshold;
}

void VolumeRenderer::SetNumSamples(uint numSamples)
{
    m_numSamples = numSamples;
}

const Path &VolumeRenderer::GetModelPath() const
{
    return m_modelPath;
}

float VolumeRenderer::GetDensityThreshold() const
{
    return m_densityThreshold;
}

uint VolumeRenderer::GetNumSamples() const
{
    return m_numSamples;
}

Texture3D *VolumeRenderer::GetVolumeTexture() const
{
    return p_volumeTexture.Get();
}

void VolumeRenderer::OnRender()
{
    Renderer::OnRender();

    GetVolumeRenderMaterial()->Bind();
    if (ShaderProgram *sp = GetVolumeRenderMaterial()->GetShaderProgram())
    {
        GetActiveMaterial()->BindMaterialUniforms(sp);

        // First pass for cube back faces
        GL::Push(GL::Pushable::FRAMEBUFFER_AND_READ_DRAW_ATTACHMENTS);
        {
            m_cubeBackFacesGBuffer->Bind();
            m_cubeBackFacesGBuffer->Resize(GL::GetViewportSize());
            m_cubeBackFacesGBuffer->SetAllDrawBuffers();
            GL::ClearDepthBuffer();
            sp->SetBool("B_RenderingCubeBackFaces", true);
            sp->SetTexture2D("B_CubeBackFacesColor",
                             TextureFactory::GetWhiteTexture());
            sp->SetTexture3D("B_Texture3D",
                             TextureFactory::GetWhiteTexture3D());

            sp->SetTexture2D(GLUniforms::UniformName_BRDF_LUT,
                             TextureFactory::GetBRDFLUTTexture());
            sp->SetTextureCubeMap("B_ReflectionProbeDiffuse",
                                  TextureFactory::GetWhiteTextureCubeMap());
            sp->SetTextureCubeMap("B_ReflectionProbeSpecular",
                                  TextureFactory::GetWhiteTextureCubeMap());


            GL::SetCullFace(GL::Face::FRONT);
            GL::Render(GetCubeMesh()->GetVAO(),
                       GL::Primitive::TRIANGLES,
                       GetCubeMesh()->GetNumVerticesIds(),
                       0);
        }
        GL::Pop(GL::Pushable::FRAMEBUFFER_AND_READ_DRAW_ATTACHMENTS);

        // Second pass
        GL::Push(GL::Pushable::DEPTH_STATES);
        {
            sp->SetFloat("B_DensityThreshold", GetDensityThreshold());
            sp->SetVector3(
                "B_Texture3DSize",
                Vector3(GetVolumeTexture() ? GetVolumeTexture()->GetSize()
                                           : Vector3i::One()));
            sp->SetVector3(
                "B_Texture3DPOTSize",
                Vector3(GetVolumeTexture() ? GetVolumeTexture()->GetSizePOT()
                                           : Vector3i::One()));

            sp->SetTexture2D(
                "B_CubeBackFacesColor",
                m_cubeBackFacesGBuffer->GetAttachmentTex2D(GBuffer::AttColor0));

            sp->SetTexture3D("B_Texture3D", GetVolumeTexture());
            sp->SetInt("B_NumSamples", GetNumSamples());
            sp->SetBool("B_RenderingCubeBackFaces", false);
            GL::SetCullFace(GL::Face::BACK);
            GL::Render(GetCubeMesh()->GetVAO(),
                       GL::Primitive::TRIANGLES,
                       GetCubeMesh()->GetNumVerticesIds(),
                       0);
        }
        GL::Pop(GL::Pushable::DEPTH_STATES);
    }
}

void VolumeRenderer::Reflect()
{
    Renderer::Reflect();

    ReflectVarMember<VolumeRenderer, float>(
        "Density threshold",
        &VolumeRenderer::SetDensityThreshold,
        &VolumeRenderer::GetDensityThreshold,
        this,
        BANG_REFLECT_HINT_SLIDER(0.01f, 0.99f));

    ReflectVarMember<VolumeRenderer, uint>("Num Samples",
                                           &VolumeRenderer::SetNumSamples,
                                           &VolumeRenderer::GetNumSamples,
                                           this,
                                           BANG_REFLECT_HINT_MIN_VALUE(1.0f));

    ReflectVarMember<VolumeRenderer, Path>(
        "Volume Path",
        &VolumeRenderer::SetModelPath,
        &VolumeRenderer::GetModelPath,
        this,
        BANG_REFLECT_HINT_EXTENSIONS(Array<String>({"txt"})));
}

Mesh *VolumeRenderer::GetCubeMesh() const
{
    return p_cubeMesh.Get();
}

Material *VolumeRenderer::GetVolumeRenderMaterial() const
{
    return m_volumeRenderingMaterial.Get();
}
