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

vec3 GetCameraSkyBoxSample(samplerCube cubeMap, vec3 direction)
{
    vec3 color;
    switch (B_Camera_ClearMode)
    {
        case CAMERA_CLEARMODE_COLOR:
            color = B_Camera_ClearColor.rgb;
        break;

        case CAMERA_CLEARMODE_SKYBOX:
            color = texture(cubeMap, direction).rgb;
        break;

        default: color = vec3(1,0,1); break;
    }
    return color;
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

    vec3 normal = B_FIn_Normal.xyz;
    if (B_HasNormalMapTexture)
    {
        vec3 normalFromMap = texture(B_NormalMapTexture, B_FIn_NormalMapUv).xyz;
        normalFromMap.xy = normalFromMap.xy * 2.0f - 1.0f;
        normalFromMap = B_TBN * normalFromMap;
        normal = normalize(normalFromMap);
    }
    B_GIn_Normal = vec4(normal * 0.5f + 0.5f, 0);

    float receivesLighting = B_MaterialReceivesLighting ? 0.25 : 0;
    if (receivesLighting > 0 && B_ReceivesShadows) { receivesLighting = 0.75; }
    B_GIn_Misc   = vec4(receivesLighting,
                        B_MaterialRoughness,
                        B_MaterialMetalness,
                        0);

    if (B_MaterialReceivesLighting)
    {
        vec3 N = B_GIn_Normal.xyz;
        vec3 V = normalize(B_Camera_WorldPos - B_FIn_Position.xyz);
        vec3 R = reflect(-V, N);

        // Calculate ambient color
        vec3 F0  = mix(vec3(0.04), B_GIn_Albedo.rgb, B_MaterialMetalness);
        vec3 FSR = FresnelSchlickRoughness(max(dot(N, V), 0.0), F0,
                                           B_MaterialRoughness);

        vec3 specK = FSR;
        vec3 diffK = (1.0 - specK) * (1.0 - B_MaterialMetalness);

        vec3 diffuseAmbient  = GetCameraSkyBoxSample(B_SkyBoxDiffuse, N) *
                               B_GIn_Albedo.rgb;
        vec3 specularAmbient = mix(GetCameraSkyBoxSample(B_SkyBoxSpecular, R),
                                   GetCameraSkyBoxSample(B_SkyBoxDiffuse,  R),
                                   B_MaterialRoughness);
        vec3 ambient = (diffK * diffuseAmbient) + (specK * specularAmbient);
        ambient *= B_AmbientLight;

        B_GIn_Color = vec4(ambient, 1);
    }
    else
    {
        B_GIn_Color = B_GIn_Albedo;
    }
}
