#include "Bang/ShaderProgramFactory.h"

USING_NAMESPACE_BANG

Path ShaderProgramFactory::GetDefaultVertexShaderPath(RenderPass renderPass)
{
    switch (renderPass)
    {
        case RenderPass::SCENE:
            return EPATH("Shaders/Default.vert");
        break;

        case RenderPass::SCENE_TRANSPARENT:
            return EPATH("Shaders/DefaultTransparent.vert");
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
            return EPATH("Shaders/Default.frag");
        break;

        case RenderPass::SCENE_TRANSPARENT:
            return EPATH("Shaders/DefaultTransparent.frag");
        break;

        default: break;
    }
    return ShaderProgramFactory::GetDefaultFragmentShaderPath(RenderPass::SCENE);
}

Path ShaderProgramFactory::GetScreenPassVertexShaderPath()
{
    return EPATH("Shaders/ScreenPass.vert");
}

ShaderProgram *ShaderProgramFactory::GetDefault(RenderPass renderPass)
{
    return Get(ShaderProgramFactory::GetDefaultVertexShaderPath(renderPass),
               ShaderProgramFactory::GetDefaultFragmentShaderPath(renderPass));
}

ShaderProgram *ShaderProgramFactory::GetDefaultPostProcess()
{
    return Get(ShaderProgramFactory::GetScreenPassVertexShaderPath(),
               EPATH("Shaders/Blur.frag"));
}

ShaderProgram *ShaderProgramFactory::GetPointLightShadowMap()
{
    return Get(EPATH("Shaders/PointLightShadowMap.vert"),
               EPATH("Shaders/PointLightShadowMap.geom"),
               EPATH("Shaders/PointLightShadowMap.frag"));
}

ShaderProgram *ShaderProgramFactory::GetPointLightScreenPass()
{
    return Get(ShaderProgramFactory::GetScreenPassVertexShaderPath(),
               EPATH("Shaders/PointLight.frag"));
}

ShaderProgram *ShaderProgramFactory::GetRenderTextureToViewport()
{
    return Get(ShaderProgramFactory::GetScreenPassVertexShaderPath(),
               EPATH("Shaders/RenderTexture.frag"));
}

ShaderProgram *ShaderProgramFactory::GetDirectionalLightScreenPass()
{
    return Get(ShaderProgramFactory::GetScreenPassVertexShaderPath(),
               EPATH("Shaders/DirectionalLight.frag"));
}

ShaderProgram *ShaderProgramFactory::Get(const Path &vShaderPath,
                                         const Path &fShaderPath)
{
    return Get(vShaderPath, Path::Empty, fShaderPath, false);
}

ShaderProgram *ShaderProgramFactory::Get(const Path &vShaderPath,
                                         const Path &gShaderPath,
                                         const Path &fShaderPath)
{
    return Get(vShaderPath, gShaderPath, fShaderPath, true);
}

ShaderProgram *ShaderProgramFactory::Get(const Path &vShaderPath,
                                         const Path &gShaderPath,
                                         const Path &fShaderPath,
                                         bool useGeometryShader)
{
    ShaderProgramFactory *spf = ShaderProgramFactory::GetActive();

    std::tuple<Path, Path, Path> shaderPathsTuple =
                        std::make_tuple(vShaderPath, gShaderPath, fShaderPath);

    if ( !spf->m_cache.ContainsKey(shaderPathsTuple) )
    {
        RH<ShaderProgram> shaderProgram;
        if (useGeometryShader)
        {
            shaderProgram = Resources::Create<ShaderProgram>(vShaderPath,
                                                             gShaderPath,
                                                             fShaderPath);
        }
        else
        {
            shaderProgram = Resources::Create<ShaderProgram>(vShaderPath,
                                                             fShaderPath);
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
