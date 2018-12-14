#include "Bang/ShaderProgramFactory.h"

#include "Bang/Map.tcc"
#include "Bang/Paths.h"
#include "Bang/Resources.h"
#include "Bang/Resources.tcc"
#include "Bang/ShaderProgram.h"

using namespace Bang;

Path ShaderProgramFactory::GetDefaultVertexShaderPath(RenderPass renderPass)
{
    switch (renderPass)
    {
        case RenderPass::SCENE:
            return ShaderProgramFactory::GetEngineShadersDir().Append(
                "Default.vert");
            break;

        case RenderPass::SCENE_TRANSPARENT:
            return ShaderProgramFactory::GetEngineShadersDir().Append(
                "DefaultTransparent.vert");
            break;

        default: break;
    }
    return ShaderProgramFactory::GetDefaultVertexShaderPath(RenderPass::SCENE);
}

Path ShaderProgramFactory::GetDefaultFragmentShaderPath(RenderPass renderPass)
{
    switch (renderPass)
    {
        case RenderPass::SCENE:
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
        RenderPass::SCENE);
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
        RH<ShaderProgram> shaderProgram;
        if (useGeometryShader)
        {
            shaderProgram = Resources::Create<ShaderProgram>(
                vShaderPath, gShaderPath, fShaderPath);
        }
        else
        {
            shaderProgram =
                Resources::Create<ShaderProgram>(vShaderPath, fShaderPath);
        }
        spf->m_cache.Add(shaderPathsTuple, shaderProgram);
    }

    return spf->m_cache.Get(shaderPathsTuple).Get();
}

ShaderProgramFactory *ShaderProgramFactory::GetActive()
{
    Resources *rs = Resources::GetInstance();
    return rs ? rs->GetShaderProgramFactory() : nullptr;
}
