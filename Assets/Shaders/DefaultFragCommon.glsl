#define BANG_FRAGMENT

#include "Common.glsl"
#include "LightCommon.glsl"

#if defined(BANG_FORWARD_RENDERING)
#include "PointLight.glsl"
#include "DirectionalLight.glsl"
#endif

in vec3 B_FIn_Position;
in vec3 B_FIn_Normal;
in vec2 B_FIn_AlbedoUv;
in vec2 B_FIn_NormalMapUv;
in vec3 B_FIn_Tangent;
in mat3 B_TBN;

#if defined(BANG_FORWARD_RENDERING)

layout(location = 0) out vec4 B_GIn_Color;

#elif defined(BANG_DEFERRED_RENDERING)

layout(location = 0) out vec4 B_GIn_Color;
layout(location = 1) out vec4 B_GIn_Albedo;
layout(location = 2) out vec4 B_GIn_Normal;
layout(location = 3) out vec4 B_GIn_Misc;

#endif

vec3 GetCameraSkyBoxSampleLod(samplerCube cubeMap, vec3 direction, float lod)
{
    vec3 color;
    switch (B_Camera_ClearMode)
    {
        case CAMERA_CLEARMODE_COLOR:
            color = B_Camera_ClearColor.rgb;
        break;

        case CAMERA_CLEARMODE_SKYBOX:
            color = textureLod(cubeMap, direction, lod).rgb;
        break;

        default: color = vec3(1,0,1); break;
    }
    return color;
}

vec3 GetCameraSkyBoxSample(samplerCube cubeMap, vec3 direction)
{
    return GetCameraSkyBoxSampleLod(cubeMap, direction, 1.0);
}



void main()
{
    vec4 texColor = vec4(1);
    if (B_HasAlbedoTexture)
    {
        texColor = texture(B_AlbedoTexture, B_FIn_AlbedoUv);
        if (texColor.a < B_AlphaCutoff) { discard; }
    }
    vec4 finalAlbedo = B_MaterialAlbedoColor * texColor;

    vec3 finalNormal = (B_FIn_Normal.xyz);
    if (B_HasNormalMapTexture)
    {
        vec3 normalFromMap = texture(B_NormalMapTexture, B_FIn_NormalMapUv).xyz;
        normalFromMap.xy = (normalFromMap.xy * 2.0f - 1.0f) * B_NormalMapMultiplyFactor;
        normalFromMap = B_TBN * normalFromMap;
        finalNormal = normalFromMap;
    }
    finalNormal = normalize(finalNormal);

    float receivesLighting = B_MaterialReceivesLighting ? 0.25 : 0;
    if (receivesLighting > 0 && B_ReceivesShadows) { receivesLighting = 0.75; }

    vec4 finalColor;
    if (B_MaterialReceivesLighting)
    {
        vec3 N = finalNormal.xyz;
        vec3 V = normalize(B_Camera_WorldPos - B_FIn_Position.xyz);
        vec3 R = reflect(-V, N);

        // Calculate ambient color
        float dotNV = max(dot(N, V), 0.0);
        vec3 F0  = mix(vec3(0.04), finalAlbedo.rgb, B_MaterialMetalness);
        vec3 FSR = FresnelSchlickRoughness(dotNV, F0, B_MaterialRoughness);
        // vec3 FSR = FresnelSchlick(max(dot(N, V), 0.0), F0);

        vec3 specK = FSR;
        vec3 diffK = (1.0 - specK) * (1.0 - B_MaterialMetalness);

        const float LOD_MAX_REFLECTION = 8.0;
        float lod = B_MaterialRoughness * LOD_MAX_REFLECTION;

        vec3 diffuseCubeMapSample  = GetCameraSkyBoxSample(B_SkyBoxDiffuse, N).rgb;
        vec3 specularCubeMapSample = GetCameraSkyBoxSampleLod(B_SkyBoxSpecular, R, lod).rgb;

        vec3 diffuseAmbient = diffuseCubeMapSample * finalAlbedo.rgb;
        vec3 specularAmbient = specularCubeMapSample;
        // vec2 envBRDF  = texture(B_BRDF_LUT, vec2(dotNV, B_MaterialRoughness)).rg;

        vec3 diffuse  = diffuseAmbient * diffK;
        vec3 specular = specularAmbient * specK; // (specK * envBRDF.x + envBRDF.y);

        vec3 ambient = (diffuse) + (specular);
        finalColor = vec4(ambient, finalAlbedo.a);
    }
    else
    {
        finalColor = finalAlbedo;
    }

    #if defined(BANG_FORWARD_RENDERING) // Apply lights in forward rendering
    if (B_MaterialReceivesLighting)
    {
        vec3 lightColorApportation = vec3(0.0f);
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
                                                      B_Camera_WorldPos,
                                                      B_FIn_Position.xyz,
                                                      finalNormal.xyz,
                                                      finalAlbedo.rgb,
                                                      B_ReceivesShadows,
                                                      B_MaterialRoughness,
                                                      B_MaterialMetalness);
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
                                                      B_Camera_WorldPos,
                                                      B_FIn_Position.xyz,
                                                      finalNormal.xyz,
                                                      finalAlbedo.rgb,
                                                      B_ReceivesShadows,
                                                      B_MaterialRoughness,
                                                      B_MaterialMetalness);
                }
                break;
            }
        }
        finalColor += vec4(lightColorApportation, 0);
    }
    #endif

    #if defined(BANG_FORWARD_RENDERING)

    B_GIn_Color  = finalColor;

    #elif defined(BANG_DEFERRED_RENDERING)

    B_GIn_Color  = finalColor;
    B_GIn_Albedo = vec4(finalAlbedo.rgb, 1);
    B_GIn_Normal = vec4(finalNormal * 0.5f + 0.5f, 0);
    B_GIn_Misc   = vec4(receivesLighting,
                        B_MaterialRoughness,
                        B_MaterialMetalness,
                        0);

    #endif
}
