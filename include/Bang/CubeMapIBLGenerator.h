#ifndef CUBEMAPIBLGENERATOR_H
#define CUBEMAPIBLGENERATOR_H

#include "Bang/BangDefines.h"
#include "Bang/ResourceHandle.h"

namespace Bang
{
class Framebuffer;
class ShaderProgram;
class TextureCubeMap;

class CubeMapIBLGenerator
{
public:
    static RH<TextureCubeMap> GenerateDiffuseIBLCubeMap(
        TextureCubeMap *textureCubeMap,
        uint IBLCubeMapSize = 32,
        uint sampleCount = 20);
    static RH<TextureCubeMap> GenerateSpecularIBLCubeMap(
        TextureCubeMap *textureCubeMap,
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

    static CubeMapIBLGenerator *GetInstance();
};
}

#endif  // CUBEMAPIBLGENERATOR_H
