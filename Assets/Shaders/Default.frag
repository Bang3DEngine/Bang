#define BANG_FRAGMENT
#include "Common.glsl"
#include "LightCommon.glsl"

in vec3 B_FIn_Position;
in vec3 B_FIn_Normal;
in vec2 B_FIn_AlbedoUv;
in vec2 B_FIn_NormalMapUv;
in vec3 B_FIn_Tangent;
in mat3 B_TBN;

layout(location = 0) out vec4 B_GIn_Color;
layout(location = 1) out vec4 B_GIn_Albedo;
layout(location = 2) out vec4 B_GIn_Normal;
layout(location = 3) out vec4 B_GIn_Misc;

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
        if (texColor.a < B_AlphaCutoff) discard;
    }
    B_GIn_Albedo = B_MaterialAlbedoColor * vec4(texColor.rgb, 1);

    vec3 normal = (B_FIn_Normal.xyz);
    if (B_HasNormalMapTexture)
    {
        vec3 normalFromMap = texture(B_NormalMapTexture, B_FIn_NormalMapUv).xyz;
        normalFromMap.xy = (normalFromMap.xy * 2.0f - 1.0f) * B_NormalMapMultiplyFactor;
        normalFromMap = B_TBN * normalFromMap;
        normal = normalFromMap;
    }
    normal = normalize(normal);
    B_GIn_Normal = vec4(normal * 0.5f + 0.5f, 0);

    float receivesLighting = B_MaterialReceivesLighting ? 0.25 : 0;
    if (receivesLighting > 0 && B_ReceivesShadows) { receivesLighting = 0.75; }
    B_GIn_Misc   = vec4(receivesLighting,
                        B_MaterialRoughness,
                        B_MaterialMetalness,
                        0);

    if (B_MaterialReceivesLighting)
    {
        vec3 N = normal.xyz;
        vec3 V = normalize(B_Camera_WorldPos - B_FIn_Position.xyz);
        vec3 R = reflect(-V, N);

        // Calculate ambient color
        vec3 F0  = mix(vec3(0.04), B_GIn_Albedo.rgb, B_MaterialMetalness);
        vec3 FSR = FresnelSchlickRoughness(max(dot(N, V), 0.0), F0, B_MaterialRoughness);
        // vec3 FSR = FresnelSchlick(max(dot(N, V), 0.0), F0);

        vec3 specK = FSR;
        vec3 diffK = (1.0 - specK) * (1.0 - B_MaterialMetalness);
        specK = (1.0 - diffK);

        vec3 diffuseAmbient  = GetCameraSkyBoxSample(B_SkyBoxDiffuse, N) *
                               B_GIn_Albedo.rgb;

        const float MAX_REFLECTION_LOD = 8.0;
        float lod = B_MaterialRoughness * MAX_REFLECTION_LOD;
        vec3 specularAmbient = GetCameraSkyBoxSampleLod(B_SkyBoxSpecular,
                                                         R, lod).rgb;

        vec3 ambient = (diffK * diffuseAmbient) + (specK * specularAmbient);
        ambient *= B_AmbientLight;

        B_GIn_Color = vec4(ambient, 1);
    }
    else
    {
        B_GIn_Color = B_GIn_Albedo;
    }
}
