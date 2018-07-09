#define BANG_FRAGMENT

#include "Common.glsl"
#include "LightCommon.glsl"
#if defined (BANG_FORWARD_RENDERING)
#include "ForwardLightCommon.glsl"
#endif

in vec3 B_FIn_Position;
in vec3 B_FIn_Normal;
in vec2 B_FIn_AlbedoUv;
in vec2 B_FIn_NormalMapUv;
in vec3 B_FIn_Tangent;
in mat3 B_TBN;
in vec4 boneIdsF;

#if defined(BANG_FORWARD_RENDERING)

layout(location = 0) out vec4 B_GIn_Color;

#elif defined(BANG_DEFERRED_RENDERING)

layout(location = 0) out vec4 B_GIn_Color;
layout(location = 1) out vec4 B_GIn_Albedo;
layout(location = 2) out vec4 B_GIn_Normal;
layout(location = 3) out vec4 B_GIn_Misc;

#endif

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

    vec3 finalPosition = B_FIn_Position.xyz;
    vec4 finalColor = GetIBLAmbientColor(finalPosition,  finalNormal, finalAlbedo);

    #if defined(BANG_FORWARD_RENDERING)

    finalColor += vec4(GetForwardLightApport(B_FIn_Position.xyz,
                                             finalNormal.xyz,
                                             finalAlbedo.rgb),
                       0);
    B_GIn_Color = finalColor;

    #elif defined(BANG_DEFERRED_RENDERING)

    float receivesLighting = B_MaterialReceivesLighting ? 0.25 : 0;
    if (receivesLighting > 0 && B_ReceivesShadows) { receivesLighting = 0.75; }

    B_GIn_Color  = finalColor;
    B_GIn_Albedo = vec4(finalAlbedo.rgb, 1);
    // B_GIn_Color = vec4(boneIdsF.x, boneIdsF.y, boneIdsF.z, boneIdsF.w) / 32.0;
    // B_GIn_Albedo = B_GIn_Color;
    int b = 18; if ( abs(boneIdsF.x - b) < 0.01f || abs(boneIdsF.y - b) < 0.01f || abs(boneIdsF.z - b) < 0.01f || abs(boneIdsF.w - b) < 0.01f ) { B_GIn_Color = B_GIn_Albedo = vec4(1,0,0,1); }
    // int w = 1; if ( abs(boneIdsF.x - w) < 0.01f || abs(boneIdsF.y - w) < 0.01f || abs(boneIdsF.z - w) < 0.01f || abs(boneIdsF.w - w) < 0.01f ) { B_GIn_Color = B_GIn_Albedo = vec4(1,0,0,1); }
    // float w = 0.5; if ( boneIdsF.x > w || boneIdsF.y > w || boneIdsF.z > w || boneIdsF.w > w) { B_GIn_Color = B_GIn_Albedo = vec4(1,0,0,1); }
    // B_GIn_Color = B_GIn_Albedo = boneIdsF;
    // int w = 1; if ( boneIdsF.x > w || boneIdsF.y > w || boneIdsF.z > w || boneIdsF.w > w) { B_GIn_Color = B_GIn_Albedo = vec4(1,0,0,1); }
    B_GIn_Normal = vec4(finalNormal * 0.5f + 0.5f, 0);
    B_GIn_Misc   = vec4(receivesLighting,
                        B_MaterialRoughness,
                        B_MaterialMetalness,
                        0);

    #endif
}
