#ifndef CUBEMAPIRRADIANCEGENERATOR_H
#define CUBEMAPIRRADIANCEGENERATOR_H

#include "Bang/Bang.h"
#include "Bang/ResourceHandle.h"
#include "Bang/TextureCubeMap.h"

NAMESPACE_BANG_BEGIN

class CubeMapIrradianceGenerator
{
public:
    static RH<TextureCubeMap>
        GenerateIrradianceCubeMap(TextureCubeMap *textureCubeMap);

    CubeMapIrradianceGenerator() = delete;
    virtual ~CubeMapIrradianceGenerator() = delete;
};

NAMESPACE_BANG_END

#endif // CUBEMAPIRRADIANCEGENERATOR_H

