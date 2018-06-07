#define BANG_FRAGMENT
#define BANG_DEFERRED_RENDERING
#include "ScreenPass.frag"
#include "PointLight.glsl"

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
                                              B_Camera_WorldPos,
                                              pixelPosWorld,
                                              pixelNormalWorld,
                                              pixelAlbedo,
                                              B_SampleReceivesShadows(),
                                              pixelRoughness,
                                              pixelMetalness);
        B_GIn_Color = vec4(lightApport, 0);
    }
    else
    {
        B_GIn_Color = vec4(0);
    }
}

