#include "Bang/VolumeRenderer.h"

#include "Bang/Assets.h"
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

    p_forwardShaderProgram.Set(ShaderProgramFactory::Get(
        Paths::GetEngineAssetsDir().Append("Shaders").Append(
            "VolumeRendererForward.vert"),
        Paths::GetEngineAssetsDir().Append("Shaders").Append(
            "VolumeRendererForward.frag")));

    p_deferredShaderProgram.Set(ShaderProgramFactory::Get(
        Paths::GetEngineAssetsDir().Append("Shaders").Append(
            "VolumeRendererDeferred.vert"),
        Paths::GetEngineAssetsDir().Append("Shaders").Append(
            "VolumeRendererDeferred.frag")));

    m_volumeRenderingMaterial = Assets::Create<Material>();
    m_volumeRenderingMaterial.Get()->GetNeededUniforms().SetOn(
        NeededUniformFlag::ALL);
    GetVolumeRenderMaterial()->SetShaderProgram(p_deferredShaderProgram.Get());

    m_volumePropertiesMaterial = MaterialFactory::GetDefault();

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
        AH<Texture3D> tex3D = Assets::Load<Texture3D>(modelPath);
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

void VolumeRenderer::SetRenderCubeMin(const Vector3 &renderCubeMin)
{
    m_renderCubeMin = renderCubeMin;
}

void VolumeRenderer::SetRenderCubeMax(const Vector3 &renderCubeMax)
{
    m_renderCubeMax = renderCubeMax;
}

void VolumeRenderer::SetUseTransferFunction(bool useTransferFunction)
{
    if (useTransferFunction != GetUseTransferFunction())
    {
        m_useTransferFunction = useTransferFunction;
    }
}

void VolumeRenderer::SetSurfaceThickness(float surfaceThickness)
{
    m_surfaceThickness = surfaceThickness;
}

void VolumeRenderer::SetInvertNormals(bool invertNormals)
{
    m_invertNormals = invertNormals;
}

void VolumeRenderer::SetAlphaMultiply(float alphaMultiply)
{
    m_alphaMultiply = alphaMultiply;
}

void VolumeRenderer::SetTransferFunctionTexture(
    Texture2D *transferFunctionTexture)
{
    p_transferFunctionTexture.Set(transferFunctionTexture);
}

void VolumeRenderer::SetVolumePropertiesMaterial(
    Material *volumePropertiesMaterial)
{
    m_volumePropertiesMaterial.Set(volumePropertiesMaterial);
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

float VolumeRenderer::GetAlphaMultiply() const
{
    return m_alphaMultiply;
}

Texture2D *VolumeRenderer::GetTransferFunctionTexture() const
{
    return p_transferFunctionTexture.Get();
}

Texture3D *VolumeRenderer::GetVolumeTexture() const
{
    return p_volumeTexture.Get();
}

const Vector3 &VolumeRenderer::GetRenderCubeMin() const
{
    return m_renderCubeMin;
}
const Vector3 &VolumeRenderer::GetRenderCubeMax() const
{
    return m_renderCubeMax;
}

bool VolumeRenderer::GetUseTransferFunction() const
{
    return m_useTransferFunction;
}

float VolumeRenderer::GetSurfaceThickness() const
{
    return m_surfaceThickness;
}

bool VolumeRenderer::GetInvertNormals() const
{
    return m_invertNormals;
}

Material *VolumeRenderer::GetVolumePropertiesMaterial() const
{
    return m_volumePropertiesMaterial.Get();
}

void VolumeRenderer::OnBeforeRender()
{
    Renderer::OnBeforeRender();

    if (GetActiveMaterial())
    {
        GetMaterial()->GetShaderProgramProperties().SetRenderPass(
            GetUseTransferFunction() ? RenderPass::SCENE_TRANSPARENT
                                     : RenderPass::SCENE_OPAQUE);
    }

    GetVolumeRenderMaterial()->SetShaderProgram(
        GetUseTransferFunction() ? p_forwardShaderProgram.Get()
                                 : p_deferredShaderProgram.Get());
}

void VolumeRenderer::OnRender()
{
    Renderer::OnRender();

    GetVolumeRenderMaterial()->Bind();
    if (ShaderProgram *sp = GetVolumeRenderMaterial()->GetShaderProgram())
    {
        if (GetVolumePropertiesMaterial())
        {
            GetVolumePropertiesMaterial()->BindMaterialUniforms(sp);
        }

        sp->SetTexture2D("B_TransferFunctionTexture",
                         GetTransferFunctionTexture()
                             ? GetTransferFunctionTexture()
                             : TextureFactory::GetWhiteTexture());

        sp->SetTexture2D(
            "B_CubeBackFacesColor",
            m_cubeBackFacesGBuffer->GetAttachmentTex2D(GBuffer::AttColor));

        sp->SetTexture3D("B_Texture3D", GetVolumeTexture());

        // First pass for cube back faces
        GL::Push(GL::Pushable::FRAMEBUFFER_AND_READ_DRAW_ATTACHMENTS);
        {
            m_cubeBackFacesGBuffer->Bind();
            m_cubeBackFacesGBuffer->Resize(GL::GetViewportSize());
            m_cubeBackFacesGBuffer->SetAllDrawBuffers();
            GL::ClearDepthBuffer();
            sp->SetBool("B_RenderingCubeBackFaces", true);

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
            sp->SetFloat("B_SurfaceThickness", GetSurfaceThickness());
            sp->SetVector3(
                "B_Texture3DSize",
                Vector3(GetVolumeTexture() ? GetVolumeTexture()->GetSize()
                                           : Vector3i::One()));
            sp->SetVector3(
                "B_Texture3DPOTSize",
                Vector3(GetVolumeTexture() ? GetVolumeTexture()->GetSizePOT()
                                           : Vector3i::One()));

            sp->SetBool("B_InvertNormals", GetInvertNormals());

            sp->SetFloat("B_AlphaMultiply", GetAlphaMultiply());
            sp->SetVector3("B_RenderCubeMin", GetRenderCubeMin());
            sp->SetVector3("B_RenderCubeMax", GetRenderCubeMax());
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

    GetReflectStructPtr()
        ->GetReflectVariablePtr("Material")
        ->GetHintsPtr()
        ->Update(BANG_REFLECT_HINT_SHOWN(false));
    GetReflectStructPtr()
        ->GetReflectVariablePtr("Casts Shadows")
        ->GetHintsPtr()
        ->Update(BANG_REFLECT_HINT_SHOWN(false));
    GetReflectStructPtr()
        ->GetReflectVariablePtr("Receives Shadows")
        ->GetHintsPtr()
        ->Update(BANG_REFLECT_HINT_SHOWN(false));
    GetReflectStructPtr()
        ->GetReflectVariablePtr("Depth Mask")
        ->GetHintsPtr()
        ->Update(BANG_REFLECT_HINT_SHOWN(false));

    BANG_REFLECT_VAR_ASSET(
        "Volume Material",
        SetVolumePropertiesMaterial,
        GetVolumePropertiesMaterial,
        Material,
        BANG_REFLECT_HINT_EXTENSIONS(Extensions::GetMaterialExtension()));

    ReflectVarMember<VolumeRenderer, float>(
        "Density threshold",
        &VolumeRenderer::SetDensityThreshold,
        &VolumeRenderer::GetDensityThreshold,
        this,
        BANG_REFLECT_HINT_SLIDER(0.01f, 0.99f) +
            BANG_REFLECT_HINT_STEP_VALUE(0.01f));

    ReflectVarMember<VolumeRenderer, float>(
        "Surface Thickness",
        &VolumeRenderer::SetSurfaceThickness,
        &VolumeRenderer::GetSurfaceThickness,
        this,
        BANG_REFLECT_HINT_SLIDER(0.01f, 0.99f) +
            BANG_REFLECT_HINT_STEP_VALUE(0.01f));

    ReflectVarMember<VolumeRenderer, bool>("Invert Normals",
                                           &VolumeRenderer::SetInvertNormals,
                                           &VolumeRenderer::GetInvertNormals,
                                           this);

    ReflectVarMember<VolumeRenderer, bool>(
        "Use Transfer Function",
        &VolumeRenderer::SetUseTransferFunction,
        &VolumeRenderer::GetUseTransferFunction,
        this);

    ReflectVarMember<VolumeRenderer, Vector3>(
        "Render Cube Min",
        &VolumeRenderer::SetRenderCubeMin,
        &VolumeRenderer::GetRenderCubeMin,
        this,
        BANG_REFLECT_HINT_MINMAX_VALUE(0.01f, 0.99f) +
            BANG_REFLECT_HINT_STEP_VALUE(0.01f));

    ReflectVarMember<VolumeRenderer, Vector3>(
        "Render Cube Max",
        &VolumeRenderer::SetRenderCubeMax,
        &VolumeRenderer::GetRenderCubeMax,
        this,
        BANG_REFLECT_HINT_MINMAX_VALUE(0.01f, 0.99f) +
            BANG_REFLECT_HINT_STEP_VALUE(0.01f));

    BANG_REFLECT_VAR_ASSET(
        "Transfer function",
        SetTransferFunctionTexture,
        GetTransferFunctionTexture,
        Texture2D,
        BANG_REFLECT_HINT_EXTENSIONS(Extensions::GetImageExtensions()) +
            BANG_REFLECT_HINT_SHOWN(GetUseTransferFunction()));

    ReflectVarMember<VolumeRenderer, float>(
        "Alpha Multiply",
        &VolumeRenderer::SetAlphaMultiply,
        &VolumeRenderer::GetAlphaMultiply,
        this,
        BANG_REFLECT_HINT_MIN_VALUE(0.0f) + BANG_REFLECT_HINT_STEP_VALUE(0.1f) +
            BANG_REFLECT_HINT_SHOWN(GetUseTransferFunction()));

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
        BANG_REFLECT_HINT_EXTENSIONS(Array<String>({"txt", "dat"})));
}

Mesh *VolumeRenderer::GetCubeMesh() const
{
    return p_cubeMesh.Get();
}

Material *VolumeRenderer::GetVolumeRenderMaterial() const
{
    return m_volumeRenderingMaterial.Get();
}
