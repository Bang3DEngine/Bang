#define BANG_FRAGMENT
#define BANG_DEFERRED_RENDERING

#include "PointLight.glsl"

in vec3 B_FIn_Position;
in vec2 B_FIn_AlbedoUv;

layout(location = 0) out vec4 B_GIn_Light;

void main()
{
    if (B_SampleReceivesLight())
    {
        vec3  pixelPosWorld    = B_ComputeWorldPosition();
        vec3  pixelNormalWorld = B_SampleNormal();
        vec3  pixelAlbedo      = B_SampleAlbedoColor().rgb;
        float pixelRoughness   = B_SampleRoughness();
        float pixelMetalness   = B_SampleMetalness();
        vec3 lightApport =
                GetPointLightColorApportation(B_LightPositionWorld,
                                              B_LightRange,
                                              B_LightIntensity,
                                              B_LightColor.rgb,
                                              B_Camera_WorldPos.xyz,
                                              pixelPosWorld,
                                              pixelNormalWorld,
                                              pixelAlbedo,
                                              B_SampleReceivesShadows(),
                                              pixelRoughness,
                                              pixelMetalness);
        B_GIn_Light = vec4(lightApport, 1);
    }
    else
    {
        B_GIn_Light = vec4(0);
    }
}

