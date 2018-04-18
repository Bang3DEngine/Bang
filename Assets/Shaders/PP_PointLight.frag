#include "PP.frag"
#include "PointLight.glsl"

void main()
{
    vec4 originalColor = B_SampleColor();
    vec3 pixelPosWorld = B_ComputeWorldPosition();
    vec3 pixelNormalWorld = B_SampleNormal();

    if (B_SampleReceivesLight())
    {
        float lightness = GetFragmentLightness(pixelPosWorld, pixelNormalWorld);
        if (lightness > 0.0f)
        {
            vec4 diffColor = B_SampleAlbedoColor();
            vec3 pointLightApport = GetPointLightColorApportation(
                                          pixelPosWorld,
                                          B_SampleNormal(),
                                          diffColor.rgb,
                                          B_SampleRoughness(),
                                          B_LightPositionWorld,
                                          B_LightIntensity,
                                          B_LightRange,
                                          B_LightColor.rgb,
                                          B_GetCameraPositionWorld() );
            pointLightApport *= lightness;

            B_GIn_Color = vec4(originalColor.rgb + pointLightApport, diffColor.a);
        }
        else { discard; }
    }
    else
    {
        // Should not arrive here, because pixels that do not receive light
        // are stenciled
        // TODO: This seems not to be being stenciled, fix this in behalf of performance
        // B_GIn_Color = vec4(1,0,0,1);
        discard;
    }
}
