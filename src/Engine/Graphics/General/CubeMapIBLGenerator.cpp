#include "Bang/CubeMapIBLGenerator.h"

#include "Bang/Assets.h"
#include "Bang/Assets.tcc"
#include "Bang/Framebuffer.h"
#include "Bang/GEngine.h"
#include "Bang/GL.h"
#include "Bang/Math.h"
#include "Bang/Paths.h"
#include "Bang/ShaderProgram.h"
#include "Bang/ShaderProgramFactory.h"
#include "Bang/TextureCubeMap.h"

using namespace Bang;

CubeMapIBLGenerator::CubeMapIBLGenerator()
{
    // Create framebuffer and shader program
    m_iblFramebuffer = new Framebuffer();

    m_iblShaderProgram = ShaderProgramFactory::Get(
        EPATH("Shaders").Append("CubeMapIBLGenerator.vert"),
        EPATH("Shaders").Append("CubeMapIBLGenerator.geom"),
        EPATH("Shaders").Append("CubeMapIBLGenerator.frag"));
}

CubeMapIBLGenerator::~CubeMapIBLGenerator()
{
    delete m_iblFramebuffer;
}

AH<TextureCubeMap> CubeMapIBLGenerator::GenerateDiffuseIBLCubeMap(
    TextureCubeMap *textureCubeMap,
    uint IBLCubeMapSize,
    uint sampleCount)
{
    return GenerateIBLCubeMap(
        textureCubeMap, IBLType::DIFFUSE, IBLCubeMapSize, sampleCount);
}

AH<TextureCubeMap> CubeMapIBLGenerator::GenerateSpecularIBLCubeMap(
    TextureCubeMap *textureCubeMap,
    uint IBLCubeMapSize,
    uint sampleCount)
{
    return GenerateIBLCubeMap(
        textureCubeMap, IBLType::SPECULAR, IBLCubeMapSize, sampleCount);
}

AH<TextureCubeMap> CubeMapIBLGenerator::GenerateIBLCubeMap(
    TextureCubeMap *textureCubeMap,
    IBLType iblType,
    uint IBLCubeMapSize,
    uint sampleCount)
{
#ifdef DEBUG
    ASSERT(Math::IsPowerOfTwo(IBLCubeMapSize));
#endif

    GL::Push(GL::Pushable::VIEWPORT);
    GL::Push(GL::Enablable::CULL_FACE);
    GL::Push(GL::Pushable::COLOR_MASK);
    GL::Push(GL::Pushable::DEPTH_STATES);
    GL::Push(GL::Pushable::ALL_MATRICES);
    GL::Push(GL::BindTarget::SHADER_PROGRAM);
    GL::Push(GL::BindTarget::TEXTURE_CUBE_MAP);
    GL::Push(GL::Pushable::FRAMEBUFFER_AND_READ_DRAW_ATTACHMENTS);

    GL::Enable(GL::Enablable::TEXTURE_CUBE_MAP_SEAMLESS);

    // Create cube map
    AH<TextureCubeMap> iblCubeMapAH = Assets::Create<TextureCubeMap>();
    TextureCubeMap *iblCubeMap = iblCubeMapAH.Get();
    iblCubeMap->SetFormat(GL::ColorFormat::RGBA16F);
    iblCubeMap->Bind();
    iblCubeMap->Resize(IBLCubeMapSize);
    iblCubeMap->SetWrapMode(GL::WrapMode::CLAMP_TO_EDGE);
    if (iblType == IBLType::SPECULAR)
    {
        iblCubeMap->GenerateMipMaps();
        iblCubeMap->SetFilterMode(GL::FilterMode::TRILINEAR_LL);
    }

    CubeMapIBLGenerator *cmg = CubeMapIBLGenerator::GetInstance();
    cmg->m_iblFramebuffer->Bind();
    cmg->m_iblShaderProgram->Bind();
    cmg->m_iblShaderProgram->SetInt("B_SampleCount", sampleCount);
    cmg->m_iblShaderProgram->SetInt("B_IBLType", SCAST<int>(iblType));
    cmg->m_iblShaderProgram->SetTextureCubeMap("B_InputCubeMap",
                                               textureCubeMap);

    // Draw to cubemap
    if (iblType == IBLType::DIFFUSE)
    {
        GL::SetViewport(0, 0, IBLCubeMapSize, IBLCubeMapSize);
        cmg->m_iblFramebuffer->SetAttachmentTexture(iblCubeMap,
                                                    GL::Attachment::COLOR0);
        cmg->m_iblFramebuffer->SetAllDrawBuffers();

        GEngine::GetInstance()->RenderViewportPlane();
    }
    else
    {
        // Fill mipmap levels of the specular skybox, each one with more
        // roughness progressively
        const uint MaxMipLevels =
            Math::Round(Math::Log10(float(IBLCubeMapSize)) / Math::Log10(2.0f));
        GL::TexParameteri(iblCubeMap->GetTextureTarget(),
                          GL::TexParameter::TEXTURE_BASE_LEVEL,
                          0);
        GL::TexParameteri(iblCubeMap->GetTextureTarget(),
                          GL::TexParameter::TEXTURE_MAX_LEVEL,
                          MaxMipLevels - 1);

        for (uint mipMapLevel = 0; mipMapLevel < MaxMipLevels; ++mipMapLevel)
        {
            const float mipMapLevelF = SCAST<float>(mipMapLevel);
            const uint mipSize =
                SCAST<uint>(IBLCubeMapSize * Math::Pow(0.5f, mipMapLevelF));

            const float roughness =
                SCAST<float>(mipMapLevel) / (MaxMipLevels - 1);
            cmg->m_iblShaderProgram->SetFloat("B_InputRoughness", roughness);

            GL::SetViewport(0, 0, mipSize, mipSize);
            cmg->m_iblFramebuffer->SetAttachmentTexture(
                iblCubeMap, GL::Attachment::COLOR0, mipMapLevel);
            cmg->m_iblFramebuffer->SetAllDrawBuffers();
            GEngine::GetInstance()->RenderViewportPlane();
        }
    }

    cmg->m_iblFramebuffer->UnBind();

    // Restore OpenGL state
    GL::Pop(GL::Pushable::VIEWPORT);
    GL::Pop(GL::Pushable::COLOR_MASK);
    GL::Pop(GL::Enablable::CULL_FACE);
    GL::Pop(GL::Pushable::DEPTH_STATES);
    GL::Pop(GL::Pushable::ALL_MATRICES);
    GL::Pop(GL::BindTarget::SHADER_PROGRAM);
    GL::Pop(GL::BindTarget::TEXTURE_CUBE_MAP);
    GL::Pop(GL::Pushable::FRAMEBUFFER_AND_READ_DRAW_ATTACHMENTS);

    return iblCubeMapAH;
}

CubeMapIBLGenerator *CubeMapIBLGenerator::GetInstance()
{
    return Assets::GetInstance()->GetCubeMapIBLGenerator();
}
