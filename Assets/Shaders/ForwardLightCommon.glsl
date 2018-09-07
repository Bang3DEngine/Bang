#ifndef FORWARD_LIGHT_COMMON_GLSL
#define FORWARD_LIGHT_COMMON_GLSL

#if defined(BANG_FORWARD_RENDERING)

#include "PointLight.glsl"
#include "DirectionalLight.glsl"

vec3 GetForwardLightApport(const vec3  pixelPosWorld,
                           const vec3  pixelNormalWorld,
                           const vec3  pixelAlbedo,
                           const float pixelRoughness,
                           const float pixelMetalness)
{
    vec3 lightColorApportation = vec3(0.0f);
    if (B_MaterialReceivesLighting)
    {
        for (int i = 0; i < B_ForwardRenderingLightNumber; ++i)
        {
            int lightType = B_ForwardRenderingLightTypes[i];
            vec3 lightColor = B_ForwardRenderingLightColors[i].rgb;
            float lightIntensity = B_ForwardRenderingLightIntensities[i];
            switch (lightType)
            {
                case LIGHT_TYPE_DIRECTIONAL:
                {
                    vec3 lightDir = B_ForwardRenderingLightForwardDirs[i];
                    lightColorApportation +=
                        GetDirectionalLightColorApportation(
                                                      lightDir,
                                                      lightIntensity,
                                                      lightColor.rgb,
                                                      B_Camera_WorldPos.xyz,
                                                      pixelPosWorld,
                                                      pixelNormalWorld,
                                                      pixelAlbedo,
                                                      false,
                                                      pixelRoughness,
                                                      pixelMetalness);
                }
                break;

                case LIGHT_TYPE_POINT:
                {
                    float lightRange = B_ForwardRenderingLightRanges[i];
                    vec3 lightPosWorld = B_ForwardRenderingLightPositions[i];
                    lightColorApportation +=
                        GetPointLightColorApportation(lightPosWorld,
                                                      lightRange,
                                                      lightIntensity,
                                                      lightColor.rgb,
                                                      B_Camera_WorldPos.xyz,
                                                      pixelPosWorld,
                                                      pixelNormalWorld,
                                                      pixelAlbedo,
                                                      false,
                                                      pixelRoughness,
                                                      pixelMetalness);
                }
                break;
            }
        }
    }
    return lightColorApportation;
}
#endif

#endif
