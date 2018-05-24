#ifndef CUBEMAPIBLGENERATOR_H
#define CUBEMAPIBLGENERATOR_H

#include "Bang/Bang.h"
#include "Bang/ResourceHandle.h"
#include "Bang/TextureCubeMap.h"

NAMESPACE_BANG_BEGIN

class CubeMapIBLGenerator
{
public:
    static RH<TextureCubeMap>
        GenerateDiffuseIBLCubeMap(TextureCubeMap *textureCubeMap);
    static RH<TextureCubeMap>
        GenerateSpecularIBLCubeMap(TextureCubeMap *textureCubeMap);

    CubeMapIBLGenerator() = delete;
    virtual ~CubeMapIBLGenerator() = delete;

private:
    enum class IBLType { DIFFUSE = 0, SPECULAR };

    static RH<TextureCubeMap> GenerateIBLCubeMap(TextureCubeMap *textureCubeMap,
                                                 IBLType iblType);
};

NAMESPACE_BANG_END

#endif // CUBEMAPIBLGENERATOR_H

