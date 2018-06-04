#include "Bang/ShaderProgramFactory.h"

USING_NAMESPACE_BANG

const Path EngShadersPath()
{
    return Paths::GetEngineAssetsDir().Append("Shaders");
}

Path ShaderProgramFactory::GetDefaultVertexShaderPath()
{
    return EngShadersPath().Append("Default.vert");
}

Path ShaderProgramFactory::GetDefaultFragmentShaderPath()
{
    return EngShadersPath().Append("Default.frag");
}

Path ShaderProgramFactory::GetScreenPassVertexShaderPath()
{
    return EngShadersPath().Append("ScreenPass.vert");
}

ShaderProgram *ShaderProgramFactory::GetDefault()
{
    return Get(ShaderProgramFactory::GetDefaultVertexShaderPath(),
               ShaderProgramFactory::GetDefaultFragmentShaderPath());
}

ShaderProgram *ShaderProgramFactory::GetDefaultPostProcess()
{
    return Get(ShaderProgramFactory::GetScreenPassVertexShaderPath(),
               EngShadersPath().Append("Blur.frag"));
}

ShaderProgram *ShaderProgramFactory::GetPointLightShadowMap()
{
    return Get(EngShadersPath().Append("PointLightShadowMap.vert"),
               EngShadersPath().Append("PointLightShadowMap.geom"),
               EngShadersPath().Append("PointLightShadowMap.frag"));
}

ShaderProgram *ShaderProgramFactory::GetPointLightScreenPass()
{
    return Get(ShaderProgramFactory::GetScreenPassVertexShaderPath(),
               EngShadersPath().Append("PointLight.frag"));
}

ShaderProgram *ShaderProgramFactory::GetRenderTextureToViewport()
{
    return Get(ShaderProgramFactory::GetScreenPassVertexShaderPath(),
               EngShadersPath().Append("ByPassTexture.frag"));
}

ShaderProgram *ShaderProgramFactory::GetDirectionalLightScreenPass()
{
    return Get(ShaderProgramFactory::GetScreenPassVertexShaderPath(),
               EngShadersPath().Append("DirectionalLight.frag"));
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
