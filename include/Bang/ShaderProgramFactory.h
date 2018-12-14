#ifndef SHADERPROGRAMFACTORY_H
#define SHADERPROGRAMFACTORY_H

#include <functional>
#include <tuple>

#include "Bang/BangDefines.h"
#include "Bang/Map.h"
#include "Bang/Path.h"
#include "Bang/RenderPass.h"
#include "Bang/ResourceHandle.h"

namespace Bang
{
class ShaderProgram;

class ShaderProgramFactory
{
public:
    static Path GetDefaultVertexShaderPath(
        RenderPass renderPass = RenderPass::SCENE);
    static Path GetDefaultFragmentShaderPath(
        RenderPass renderPass = RenderPass::SCENE);
    static Path GetScreenPassVertexShaderPath();

    static ShaderProgram *GetDefault(RenderPass renderPass = RenderPass::SCENE);
    static ShaderProgram *GetDefaultPostProcess();
    static ShaderProgram *GetPointLightShadowMap();
    static ShaderProgram *GetPointLightDeferredScreenPass();
    static ShaderProgram *GetSeparableBlur();
    static ShaderProgram *GetSeparableBlurCubeMap();
    static ShaderProgram *GetRenderTextureToViewport();
    static ShaderProgram *GetRenderTextureToViewportGamma();
    static ShaderProgram *GetDirectionalLightShadowMap();
    static ShaderProgram *GetDirectionalLightDeferredScreenPass();
    static ShaderProgram *Get(const Path &vShaderPath, const Path &fShaderPath);
    static ShaderProgram *Get(const Path &vShaderPath,
                              const Path &gShaderPath,
                              const Path &fShaderPath);

    static Path GetEngineShadersDir();

private:
    Map<std::tuple<Path, Path, Path>, RH<ShaderProgram>> m_cache;

    ShaderProgramFactory() = default;

    static ShaderProgram *Get(const Path &vShaderPath,
                              const Path &gShaderPath,
                              const Path &fShaderPath,
                              bool useGeometryShader);
    static ShaderProgramFactory *GetActive();

    friend class Resources;
};
}

#endif  // SHADERPROGRAMFACTORY_H
