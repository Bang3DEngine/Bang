#define BANG_FRAGMENT
#include "Common.glsl"
#include "LightCommon.glsl"

in vec3 B_FIn_Position;
in vec3 B_FIn_Normal;
in vec2 B_FIn_Uv;

layout(location = 0) out vec4 B_GIn_Color;
layout(location = 1) out vec4 B_GIn_Albedo;
layout(location = 2) out vec4 B_GIn_Normal;
layout(location = 3) out vec4 B_GIn_Misc;

void main()
{
    vec4 texColor = vec4(1);
    if (B_HasTexture)
    {
        texColor = texture(B_Texture0, B_FIn_Uv);
        if (texColor.a < B_AlphaCutoff) discard;
    }

    B_GIn_Albedo = B_MaterialAlbedoColor * vec4(texColor.rgb, 1);
    B_GIn_Normal = vec4(B_FIn_Normal.xyz * 0.5f + 0.5f, 0);
    B_GIn_Misc   = vec4(B_MaterialReceivesLighting ? 1.0 : 0.0,
                        B_MaterialRoughness,
                        B_MaterialMetalness,
                        0);

    if (B_MaterialReceivesLighting)
    {
        vec3 N = B_FIn_Normal.xyz;
        vec3 V = normalize(B_Camera_WorldPos - B_FIn_Position.xyz);
        vec3 R = reflect(-V, N);

        // Calculate ambient color
        vec3 F0  = mix(vec3(0.04), B_GIn_Albedo.rgb, B_MaterialMetalness);
        vec3 FSR = FresnelSchlickRoughness(max(dot(N, V), 0.0), F0,
                                           B_MaterialRoughness);

        vec3 specK = FSR;
        vec3 diffK = (1.0 - specK) * (1.0 - B_MaterialMetalness);
        vec3 diffuseAmbient  = texture(B_SkyBoxDiffuse, N).rgb * B_GIn_Albedo.rgb;
        vec3 specularAmbient = mix(texture(B_SkyBoxSpecular, R).rgb,
                                   texture(B_SkyBoxDiffuse,  R).rgb,
                                   B_MaterialRoughness);
        vec3 ambient = (diffK * diffuseAmbient) + (specK * specularAmbient);
        ambient *= B_AmbientLight;

        B_GIn_Color = vec4(ambient, B_GIn_Albedo.a);
    }
    else
    {
        B_GIn_Color = B_GIn_Albedo;
    }
}
