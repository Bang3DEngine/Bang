#ifndef CUBEMAPIBLGENERATOR_H
#define CUBEMAPIBLGENERATOR_H

#include "Bang/Bang.h"
#include "Bang/Framebuffer.h"
#include "Bang/ResourceHandle.h"
#include "Bang/TextureCubeMap.h"

NAMESPACE_BANG_BEGIN

class CubeMapIBLGenerator
{
public:
    static RH<TextureCubeMap>
        GenerateDiffuseIBLCubeMap(TextureCubeMap *textureCubeMap,
                                  uint IBLCubeMapSize = 32,
                                  uint sampleCount = 20);
    static RH<TextureCubeMap>
        GenerateSpecularIBLCubeMap(TextureCubeMap *textureCubeMap,
                                   uint IBLCubeMapSize = 128,
                                   uint sampleCount = 256);

    CubeMapIBLGenerator();
    virtual ~CubeMapIBLGenerator();

private:
    enum class IBLType
    {
        DIFFUSE,
        SPECULAR
    };

    Framebuffer *m_iblFramebuffer = nullptr;
    ShaderProgram *m_iblShaderProgram = nullptr;

    static RH<TextureCubeMap> GenerateIBLCubeMap(TextureCubeMap *textureCubeMap,
                                                 IBLType iblType,
                                                 uint IBLCubeMapSize,
                                                 uint sampleCount);

    static CubeMapIBLGenerator* GetInstance();
};

NAMESPACE_BANG_END

#endif // CUBEMAPIBLGENERATOR_H

