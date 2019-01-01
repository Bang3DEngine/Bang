#include "Bang/ShaderProgramFactory.h"

#include "Bang/Assets.h"
#include "Bang/Assets.tcc"
#include "Bang/Map.tcc"
#include "Bang/Paths.h"
#include "Bang/ShaderProgram.h"

using namespace Bang;

Path ShaderProgramFactory::GetDefaultVertexShaderPath(RenderPass renderPass)
{
    switch (renderPass)
    {
        case RenderPass::SCENE_OPAQUE:
            return ShaderProgramFactory::GetEngineShadersDir().Append(
                "Default.vert");
            break;

        case RenderPass::SCENE_TRANSPARENT:
            return ShaderProgramFactory::GetEngineShadersDir().Append(
                "DefaultTransparent.vert");
            break;

        default: break;
    }
    return ShaderProgramFactory::GetDefaultVertexShaderPath(
        RenderPass::SCENE_OPAQUE);
}

Path ShaderProgramFactory::GetDefaultFragmentShaderPath(RenderPass renderPass)
{
    switch (renderPass)
    {
        case RenderPass::SCENE_OPAQUE:
            return ShaderProgramFactory::GetEngineShadersDir().Append(
                "Default.frag");
            break;

        case RenderPass::SCENE_TRANSPARENT:
            return ShaderProgramFactory::GetEngineShadersDir().Append(
                "DefaultTransparent.frag");
            break;

        default: break;
    }
    return ShaderProgramFactory::GetDefaultFragmentShaderPath(
        RenderPass::SCENE_OPAQUE);
}

Path ShaderProgramFactory::GetScreenPassVertexShaderPath()
{
    return ShaderProgramFactory::GetEngineShadersDir().Append(
        "ScreenPass.vert");
}

ShaderProgram *ShaderProgramFactory::GetDefault(RenderPass renderPass)
{
    return Get(ShaderProgramFactory::GetDefaultVertexShaderPath(renderPass),
               ShaderProgramFactory::GetDefaultFragmentShaderPath(renderPass));
}

ShaderProgram *ShaderProgramFactory::GetDefaultPostProcess()
{
    return Get(ShaderProgramFactory::GetScreenPassVertexShaderPath(),
               ShaderProgramFactory::GetEngineShadersDir().Append("Blur.frag"));
}

ShaderProgram *ShaderProgramFactory::GetPointLightShadowMap()
{
    return Get(ShaderProgramFactory::GetEngineShadersDir().Append(
                   "PointLightShadowMap.vert"),
               ShaderProgramFactory::GetEngineShadersDir().Append(
                   "PointLightShadowMap.geom"),
               ShaderProgramFactory::GetEngineShadersDir().Append(
                   "PointLightShadowMap.frag"));
}

ShaderProgram *ShaderProgramFactory::GetPointLightDeferredScreenPass()
{
    return Get(ShaderProgramFactory::GetScreenPassVertexShaderPath(),
               ShaderProgramFactory::GetEngineShadersDir().Append(
                   "PointLightDeferred.frag"));
}

ShaderProgram *ShaderProgramFactory::GetDecal()
{
    return Get(
        ShaderProgramFactory::GetEngineShadersDir().Append("Decal.vert"),
        ShaderProgramFactory::GetEngineShadersDir().Append("Decal.frag"));
}

ShaderProgram *ShaderProgramFactory::GetKawaseBlur()
{
    return Get(
        ShaderProgramFactory::GetScreenPassVertexShaderPath(),
        ShaderProgramFactory::GetEngineShadersDir().Append("KawaseBlur.frag"));
}

ShaderProgram *ShaderProgramFactory::GetSeparableBlur()
{
    return Get(ShaderProgramFactory::GetScreenPassVertexShaderPath(),
               ShaderProgramFactory::GetEngineShadersDir().Append(
                   "SeparableBlur.frag"));
}

ShaderProgram *ShaderProgramFactory::GetSeparableBlurCubeMap()
{
    return Get(ShaderProgramFactory::GetScreenPassVertexShaderPath(),
               ShaderProgramFactory::GetEngineShadersDir().Append(
                   "SeparableBlurCM.frag"));
}

ShaderProgram *ShaderProgramFactory::GetRenderTextureToViewport()
{
    return Get(ShaderProgramFactory::GetScreenPassVertexShaderPath(),
               ShaderProgramFactory::GetEngineShadersDir().Append(
                   "RenderTexture.frag"));
}

ShaderProgram *ShaderProgramFactory::GetRenderTextureToViewportGamma()
{
    return Get(ShaderProgramFactory::GetScreenPassVertexShaderPath(),
               ShaderProgramFactory::GetEngineShadersDir().Append(
                   "RenderTextureGamma.frag"));
}

ShaderProgram *ShaderProgramFactory::GetDirectionalLightShadowMap()
{
    return Get(ShaderProgramFactory::GetEngineShadersDir().Append(
                   "DirectionalLightShadowMap.vert"),
               ShaderProgramFactory::GetEngineShadersDir().Append(
                   "DirectionalLightShadowMap.frag"));
}

ShaderProgram *ShaderProgramFactory::GetDirectionalLightDeferredScreenPass()
{
    return Get(ShaderProgramFactory::GetScreenPassVertexShaderPath(),
               ShaderProgramFactory::GetEngineShadersDir().Append(
                   "DirectionalLightDeferred.frag"));
}

ShaderProgram *ShaderProgramFactory::Get(const Path &vShaderPath,
                                         const Path &fShaderPath)
{
    return Get(vShaderPath, Path::Empty(), fShaderPath, false);
}

ShaderProgram *ShaderProgramFactory::Get(const Path &vShaderPath,
                                         const Path &gShaderPath,
                                         const Path &fShaderPath)
{
    return Get(vShaderPath, gShaderPath, fShaderPath, true);
}

ShaderProgram *ShaderProgramFactory::Get(const Path &shaderPath)
{
    ShaderProgramFactory *spf = ShaderProgramFactory::GetActive();
    if (!spf->m_shaderCache.ContainsKey(shaderPath))
    {
        AH<ShaderProgram> shaderProgram;
        shaderProgram = Assets::Create<ShaderProgram>();
        shaderProgram.Get()->Load(shaderPath);
        spf->m_shaderCache.Add(shaderPath, shaderProgram);
    }
    return spf->m_shaderCache.Get(shaderPath).Get();
}

Path ShaderProgramFactory::GetEngineShadersDir()
{
    return EPATH("Shaders");
}

ShaderProgram *ShaderProgramFactory::Get(const Path &vShaderPath,
                                         const Path &gShaderPath,
                                         const Path &fShaderPath,
                                         bool useGeometryShader)
{
    ShaderProgramFactory *spf = ShaderProgramFactory::GetActive();

    std::tuple<Path, Path, Path> shaderPathsTuple =
        std::make_tuple(vShaderPath, gShaderPath, fShaderPath);

    if (!spf->m_cache.ContainsKey(shaderPathsTuple))
    {
        AH<ShaderProgram> shaderProgram;
        if (useGeometryShader)
        {
            shaderProgram = Assets::Create<ShaderProgram>(
                vShaderPath, gShaderPath, fShaderPath);
        }
        else
        {
            shaderProgram =
                Assets::Create<ShaderProgram>(vShaderPath, fShaderPath);
        }
        spf->m_cache.Add(shaderPathsTuple, shaderProgram);
    }

    return spf->m_cache.Get(shaderPathsTuple).Get();
}

ShaderProgramFactory *ShaderProgramFactory::GetActive()
{
    Assets *rs = Assets::GetInstance();
    return rs ? rs->GetShaderProgramFactory() : nullptr;
}
