#include "Bang/CubeMapIBLGenerator.h"

#include "Bang/Mesh.h"
#include "Bang/Scene.h"
#include "Bang/AARect.h"
#include "Bang/Shader.h"
#include "Bang/Camera.h"
#include "Bang/Matrix4.h"
#include "Bang/GEngine.h"
#include "Bang/Resources.h"
#include "Bang/GameObject.h"
#include "Bang/GLUniforms.h"
#include "Bang/Framebuffer.h"
#include "Bang/TextureFactory.h"
#include "Bang/MeshFactory.h"
#include "Bang/MeshRenderer.h"
#include "Bang/SceneManager.h"
#include "Bang/ShaderProgram.h"
#include "Bang/GameObjectFactory.h"
#include "Bang/ShaderProgramFactory.h"

USING_NAMESPACE_BANG

RH<TextureCubeMap> CubeMapIBLGenerator::GenerateDiffuseIBLCubeMap(
                                                TextureCubeMap *textureCubeMap)
{
    return GenerateIBLCubeMap(textureCubeMap, IBLType::Diffuse);
}

RH<TextureCubeMap> CubeMapIBLGenerator::GenerateSpecularIBLCubeMap(
                                                TextureCubeMap *textureCubeMap)
{
    return GenerateIBLCubeMap(textureCubeMap, IBLType::Specular);
}

RH<TextureCubeMap> CubeMapIBLGenerator::GenerateIBLCubeMap(
                                                TextureCubeMap *textureCubeMap,
                                                IBLType iblType)
{
    constexpr uint IBLCubeMapSizes[2] = {32, 128};
    const uint IBLCubeMapSize = IBLCubeMapSizes[iblType];

    // Save OpenGL state
    const AARecti prevVP = GL::GetViewportRect();
    const Matrix4 &prevModel = GLUniforms::GetModelMatrix();
    const Matrix4 &prevView  = GLUniforms::GetViewMatrix();
    const Matrix4 &prevProj  = GLUniforms::GetProjectionMatrix();
    const GLId prevBoundFB   = GL::GetBoundId(GL::BindTarget::Framebuffer);
    const GLId prevBoundSP   = GL::GetBoundId(GL::BindTarget::ShaderProgram);
    const GLId prevBoundTex  = GL::GetBoundId(GL::BindTarget::TextureCubeMap);
    bool wasCullEnabled      = GL::IsEnabled(GL::Enablable::CullFace);

    GL::Enable(GL::Enablable::TextureCubeMapSeamless);

    // Create cube map
    RH<TextureCubeMap> iblCubeMapRH = Resources::Create<TextureCubeMap>();
    TextureCubeMap *iblCubeMap = iblCubeMapRH.Get();
    iblCubeMap->SetFormat(GL::ColorFormat::RGBA_Float16);
    iblCubeMap->Bind();
    iblCubeMap->Resize(IBLCubeMapSize);
    iblCubeMap->SetWrapMode(GL::WrapMode::ClampToEdge);
    if (iblType == IBLType::Specular)
    {
        iblCubeMap->GenerateMipMaps();
        iblCubeMap->SetFilterMode(GL::FilterMode::Trilinear_LL);
    }

    // Create shader program
    const Path engShadersDir = Paths::GetEngineAssetsDir().Append("Shaders");
    ShaderProgram *sp = ShaderProgramFactory::Get(
                            engShadersDir.Append("CubeMapIBLGenerator.vert"),
                            engShadersDir.Append("CubeMapIBLGenerator.geom"),
                            engShadersDir.Append("CubeMapIBLGenerator.frag"));

    // Create framebuffer and shader program
    Framebuffer *fb = new Framebuffer();
    fb->Bind();

    sp->Bind();
    sp->SetInt("B_IBLType", iblType);
    sp->SetTextureCubeMap("B_InputCubeMap", textureCubeMap);

    // Draw to cubemap
    if (iblType == IBLType::Diffuse)
    {
        GL::SetViewport(0, 0, IBLCubeMapSize, IBLCubeMapSize);
        fb->SetAttachmentTexture(iblCubeMap, GL::Attachment::Color0);
        fb->SetAllDrawBuffers();

        GEngine::GetActive()->RenderViewportPlane();
    }
    else
    {
        // Fill mipmap levels of the specular skybox, each one with more
        // roughness progressively
        constexpr int MaxMipLevels = 8;
        GL::TexParameteri(iblCubeMap->GetTextureTarget(),
                          GL::TexParameter::TEXTURE_BASE_LEVEL, 0);
        GL::TexParameteri(iblCubeMap->GetTextureTarget(),
                          GL::TexParameter::TEXTURE_MAX_LEVEL, MaxMipLevels-1);

        for (int mipMapLevel = 0; mipMapLevel < MaxMipLevels; ++mipMapLevel)
        {
            const float mipMapLevelF = SCAST<float>(mipMapLevel);
            const uint mipSize = SCAST<uint>(IBLCubeMapSize * Math::Pow(0.5f, mipMapLevelF));

            const float roughness = SCAST<float>(mipMapLevel) / (MaxMipLevels - 1);
            sp->SetFloat("B_InputRoughness", roughness);

            GL::SetViewport(0, 0, mipSize, mipSize);
            fb->SetAttachmentTexture(iblCubeMap, GL::Attachment::Color0, mipMapLevel);
            fb->SetAllDrawBuffers();
            GEngine::GetActive()->RenderViewportPlane();
        }
    }

    fb->UnBind();
    delete fb;

    // Restore OpenGL state
    GL::SetViewport(prevVP);
    GL::SetEnabled(GL::Enablable::CullFace, wasCullEnabled);
    GL::SetColorMask(true, true, true, true);
    GLUniforms::SetModelMatrix(prevModel);
    GLUniforms::SetViewMatrix(prevView);
    GLUniforms::SetProjectionMatrix(prevProj);
    GL::Bind(GL::BindTarget::Framebuffer,    prevBoundFB);
    GL::Bind(GL::BindTarget::ShaderProgram,  prevBoundSP);
    GL::Bind(GL::BindTarget::TextureCubeMap, prevBoundTex);

    return iblCubeMapRH;
}









