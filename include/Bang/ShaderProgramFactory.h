#ifndef SHADERPROGRAMFACTORY_H
#define SHADERPROGRAMFACTORY_H

#include <tuple>

#include "Bang/RenderPass.h"
#include "Bang/ShaderProgram.h"
#include "Bang/ResourceHandle.h"

NAMESPACE_BANG_BEGIN

class ShaderProgramFactory
{
public:
    static Path GetDefaultVertexShaderPath(RenderPass renderPass = RenderPass::SCENE);
    static Path GetDefaultFragmentShaderPath(RenderPass renderPass = RenderPass::SCENE);
    static Path GetScreenPassVertexShaderPath();

    static ShaderProgram* GetDefault(RenderPass renderPass = RenderPass::SCENE);
    static ShaderProgram* GetDefaultPostProcess();
    static ShaderProgram* GetPointLightShadowMap();
    static ShaderProgram* GetPointLightScreenPass();
    static ShaderProgram* GetRenderTextureToViewport();
    static ShaderProgram* GetDirectionalLightScreenPass();
    static ShaderProgram* Get(const Path &vShaderPath, const Path &fShaderPath);
    static ShaderProgram* Get(const Path &vShaderPath,
                              const Path &gShaderPath,
                              const Path &fShaderPath);

private:
    Map< std::tuple<Path,Path,Path>, RH<ShaderProgram> > m_cache;

    ShaderProgramFactory() = default;

    static ShaderProgram* Get(const Path &vShaderPath,
                              const Path &gShaderPath,
                              const Path &fShaderPath,
                              bool useGeometryShader);
    static ShaderProgramFactory* GetActive();

    friend class Resources;
};

NAMESPACE_BANG_END

#endif // SHADERPROGRAMFACTORY_H

