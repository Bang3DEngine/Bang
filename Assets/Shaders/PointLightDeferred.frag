#define BANG_FRAGMENT
#define BANG_DEFERRED_RENDERING

#define BANG_UNIFORMS_SKYBOXES
#define BANG_DEFERRED_RENDERING
#define BANG_UNIFORMS_MATERIAL_PBR
#define BANG_UNIFORMS_GBUFFER_TEXTURES
#define BANG_UNIFORMS_REFLECTION_PROBES

#include "PointLight.glsl"

in vec3 B_FIn_Position;
in vec2 B_FIn_AlbedoUv;

layout(location = 0) out vec4 B_GIn_Light;

void main()
{
    vec2 uv = B_FIn_AlbedoUv;
    if (B_SampleReceivesLight(uv))
    {
        vec3  pixelPosWorld    = B_ComputeWorldPosition(uv);
        vec3  pixelNormalWorld = B_SampleNormal(uv);
        vec3  pixelAlbedo      = B_SampleAlbedoColor(uv).rgb;
        float pixelRoughness   = B_SampleRoughness(uv);
        float pixelMetalness   = B_SampleMetalness(uv);
        vec3 lightApport =
                GetPointLightColorApportation(B_LightPositionWorld,
                                              B_LightRange,
                                              B_LightIntensity,
                                              B_LightColor.rgb,
                                              B_Camera_WorldPos.xyz,
                                              pixelPosWorld,
                                              pixelNormalWorld,
                                              pixelAlbedo,
                                              B_SampleReceivesShadows(uv),
                                              pixelRoughness,
                                              pixelMetalness);
        B_GIn_Light = vec4(lightApport, 1);
    }
    else
    {
        B_GIn_Light = vec4(0);
    }
}

