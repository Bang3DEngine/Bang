#define BANG_FRAGMENT
#define BANG_DEFERRED_RENDERING
#define BANG_USE_REFLECTION_PROBES
#include "DirectionalLight.glsl"

in vec3 B_FIn_Position;
in vec2 B_FIn_AlbedoUv;

layout(location = 0) out vec4 B_GIn_Light;

vec3 GetLightColorApportation()
{
    vec3  pixelPosWorld    = B_ComputeWorldPosition();
    vec3  pixelNormalWorld = B_SampleNormal();
    vec3  pixelAlbedo      = B_SampleAlbedoColor().rgb;
    float pixelRoughness   = B_SampleRoughness();
    float pixelMetalness   = B_SampleMetalness();

    return GetDirectionalLightColorApportation(B_LightForwardWorld,
                                               B_LightIntensity,
                                               B_LightColor.rgb,
                                               B_Camera_WorldPos.xyz,
                                               pixelPosWorld,
                                               pixelNormalWorld,
                                               pixelAlbedo,
                                               B_SampleReceivesShadows(),
                                               pixelRoughness,
                                               pixelMetalness);
}

void main()
{
    if (B_SampleReceivesLight())
    {
        vec3 lightApport = GetLightColorApportation();
        B_GIn_Light = vec4(lightApport, 1);
    }
    else
    {
        B_GIn_Light = vec4(0);
    }
}


