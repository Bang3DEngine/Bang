#define BANG_FRAGMENT
#define BANG_DEFERRED_RENDERING
#include "ScreenPass.frag"
#include "DirectionalLight.glsl"

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
                                               B_Camera_WorldPos,
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
        B_GIn_Color = vec4(lightApport, 0);
    }
    else
    {
        B_GIn_Color = vec4(0);
    }
}


