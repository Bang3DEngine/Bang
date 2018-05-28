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
    return GenerateIBLCubeMap(textureCubeMap, IBLType::DIFFUSE);
}

RH<TextureCubeMap> CubeMapIBLGenerator::GenerateSpecularIBLCubeMap(
                                                TextureCubeMap *textureCubeMap)
{
    return GenerateIBLCubeMap(textureCubeMap, IBLType::SPECULAR);
}

RH<TextureCubeMap> CubeMapIBLGenerator::GenerateIBLCubeMap(
                                                TextureCubeMap *textureCubeMap,
                                                IBLType iblType)
{
    constexpr uint IBLCubeMapSizes[2] = {32, 128};
    const uint IBLCubeMapSize = IBLCubeMapSizes[ SCAST<int>(iblType) ];

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
    RH<TextureCubeMap> iblCubeMapRH = Resources::Create<TextureCubeMap>();
    TextureCubeMap *iblCubeMap = iblCubeMapRH.Get();
    iblCubeMap->SetFormat(GL::ColorFormat::RGBA16F);
    iblCubeMap->Bind();
    iblCubeMap->Resize(IBLCubeMapSize);
    iblCubeMap->SetWrapMode(GL::WrapMode::CLAMP_TO_EDGE);
    if (iblType == IBLType::SPECULAR)
    {
        iblCubeMap->GenerateMipMaps();
        iblCubeMap->SetFilterMode(GL::FilterMode::TRILINEAR_LL);
    }

    // Create shader program
    ShaderProgram *sp = ShaderProgramFactory::Get(
                            EPATH("Shaders/CubeMapIBLGenerator.vert"),
                            EPATH("Shaders/CubeMapIBLGenerator.geom"),
                            EPATH("Shaders/CubeMapIBLGenerator.frag"));

    // Create framebuffer and shader program
    Framebuffer *fb = new Framebuffer();
    fb->Bind();

    sp->Bind();
    sp->SetInt("B_IBLType", SCAST<int>(iblType));
    sp->SetTextureCubeMap("B_InputCubeMap", textureCubeMap);

    // Draw to cubemap
    if (iblType == IBLType::DIFFUSE)
    {
        GL::SetViewport(0, 0, IBLCubeMapSize, IBLCubeMapSize);
        fb->SetAttachmentTexture(iblCubeMap, GL::Attachment::COLOR0);
        fb->SetAllDrawBuffers();

        GEngine::GetInstance()->RenderViewportPlane();
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
            const uint mipSize = IBLCubeMapSize * Math::Pow(0.5f, mipMapLevelF);

            const float roughness = SCAST<float>(mipMapLevel) / (MaxMipLevels - 1);
            sp->SetFloat("B_InputRoughness", roughness);

            GL::SetViewport(0, 0, mipSize, mipSize);
            fb->SetAttachmentTexture(iblCubeMap, GL::Attachment::COLOR0, mipMapLevel);
            fb->SetAllDrawBuffers();
            GEngine::GetInstance()->RenderViewportPlane();
        }
    }

    fb->UnBind();
    delete fb;

    // Restore OpenGL state
    GL::Pop(GL::Pushable::VIEWPORT);
    GL::Pop(GL::Pushable::COLOR_MASK);
    GL::Pop(GL::Enablable::CULL_FACE);
    GL::Pop(GL::Pushable::DEPTH_STATES);
    GL::Pop(GL::Pushable::ALL_MATRICES);
    GL::Pop(GL::BindTarget::SHADER_PROGRAM);
    GL::Pop(GL::BindTarget::TEXTURE_CUBE_MAP);
    GL::Pop(GL::Pushable::FRAMEBUFFER_AND_READ_DRAW_ATTACHMENTS);

    return iblCubeMapRH;
}









