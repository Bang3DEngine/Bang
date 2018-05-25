#include "Bang/ShaderProgramFactory.h"

USING_NAMESPACE_BANG

Path ShaderProgramFactory::GetDefaultVertexShaderPath()
{
    return EPATH("Shaders/Default.vert");
}

Path ShaderProgramFactory::GetDefaultFragmentShaderPath()
{
    return EPATH("Shaders/Default.frag");
}

Path ShaderProgramFactory::GetScreenPassVertexShaderPath()
{
    return EPATH("Shaders/ScreenPass.vert");
}

ShaderProgram *ShaderProgramFactory::GetDefault()
{
    return Get(ShaderProgramFactory::GetDefaultVertexShaderPath(),
               ShaderProgramFactory::GetDefaultFragmentShaderPath());
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
    Resources *rs = Resources::GetActive();
    return rs ? rs->GetShaderProgramFactory() : nullptr;
}
