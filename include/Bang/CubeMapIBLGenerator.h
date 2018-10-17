#ifndef CUBEMAPIBLGENERATOR_H
#define CUBEMAPIBLGENERATOR_H

#include "Bang/BangDefines.h"
#include "Bang/ResourceHandle.h"

NAMESPACE_BANG_BEGIN

FORWARD class Framebuffer;
FORWARD class ShaderProgram;
FORWARD class TextureCubeMap;

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

