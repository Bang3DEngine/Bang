#define BANG_FRAGMENT
#define BANG_USE_REFLECTION_PROBES

#include "Common.glsl"
#include "LightCommon.glsl"
#include "MaterialPBRUniforms.glsl"

#if defined BANG_FORWARD_RENDERING
    #include "ForwardLightCommon.glsl"
#endif

in vec3 B_FIn_Position;
in vec3 B_FIn_Normal;
in vec2 B_FIn_AlbedoUv;
#ifdef BANG_NORMAL_MAPPING
    in vec2 B_FIn_NormalMapUv;
    in mat3 B_TBN;
#endif

#if defined(BANG_FORWARD_RENDERING)
    layout(location = 0) out vec4 B_GIn_Color;
#elif defined(BANG_DEFERRED_RENDERING)
    layout(location = 0) out vec4 B_GIn_Color;
    layout(location = 1) out vec4 B_GIn_Albedo;
    layout(location = 2) out vec4 B_GIn_Light;
    layout(location = 3) out vec4 B_GIn_Normal;
    layout(location = 4) out vec4 B_GIn_Misc;
#endif

void DefaultFragCommonMain(vec4 albedoColor)
{
    vec4 texColor = vec4(1);
    if (B_HasAlbedoTexture)
    {
        texColor = texture(B_AlbedoTexture, B_FIn_AlbedoUv);
        if (texColor.a <= B_AlphaCutoff)
        {
            discard;
        }
    }
    vec4 finalAlbedo = albedoColor * texColor;

    vec3 finalNormal = (B_FIn_Normal.xyz);
    #ifdef BANG_NORMAL_MAPPING
        if (B_HasNormalMapTexture)
        {
            vec3 normalFromMap = texture(B_NormalMapTexture, B_FIn_NormalMapUv).xyz;
            normalFromMap.xy = (normalFromMap.xy * 2.0f - 1.0f) * B_NormalMapMultiplyFactor;
            normalFromMap = B_TBN * normalFromMap;
            finalNormal = normalFromMap;
        }
    #endif
    finalNormal = normalize(finalNormal);

    float pixelRoughness = texture(B_RoughnessTexture, B_FIn_AlbedoUv).r * B_MaterialRoughness;
    float pixelMetalness = texture(B_MetalnessTexture, B_FIn_AlbedoUv).r * B_MaterialMetalness;
    vec3 finalPosition = B_FIn_Position.xyz;
    vec4 finalColor = GetIBLAmbientColor(B_MaterialReceivesLighting,
                                         finalPosition,  finalNormal, finalAlbedo,
                                         pixelRoughness, pixelMetalness);

    #if defined(BANG_FORWARD_RENDERING)
        finalColor += vec4(GetForwardLightApport(B_FIn_Position.xyz,
                                                 finalNormal.xyz,
                                                 finalAlbedo.rgb,
                                                 pixelRoughness,
                                                 pixelMetalness), 0);
    #elif defined(BANG_DEFERRED_RENDERING)
        float receivesLighting = B_MaterialReceivesLighting ? 0.25 : 0;
        if (receivesLighting > 0 && B_ReceivesShadows)
        {
            receivesLighting = 0.75;
        }

        B_GIn_Albedo = vec4(finalAlbedo.rgb, 1);
        B_GIn_Light = vec4(0,0,0,0);
        B_GIn_Normal = vec4(finalNormal * 0.5f + 0.5f, 0);
        B_GIn_Misc   = vec4(receivesLighting,
                            pixelRoughness,
                            pixelMetalness,
                            0);
    #endif

    B_GIn_Color = finalColor;
}

#ifndef BANG_SUPPRESS_DEFAULT_FRAG_COMMON_MAIN
void main()
{
    DefaultFragCommonMain(B_MaterialAlbedoColor);
}
#endif
