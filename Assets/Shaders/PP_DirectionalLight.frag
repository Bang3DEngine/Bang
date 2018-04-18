#include "PP.frag"
#include "DirectionalLight.glsl"

void main()
{
    vec4 originalColor    = B_SampleColor();
    vec3 pixelPosWorld    = B_ComputeWorldPosition();
    vec3 pixelNormalWorld = B_SampleNormal();

    if (B_SampleReceivesLight())
    {
        // Get lightness of the pixel using shadow mapping
        vec3 camPosWorld = B_GetCameraPositionWorld();
        float lightness = GetFragmentLightness(pixelPosWorld,
                                               pixelNormalWorld,
                                               B_LightForwardWorld,
                                               camPosWorld);

        if (lightness > 0.0f)
        {
            vec4 diffColor = B_SampleAlbedoColor();
            vec3 dirLightApport = GetDirectionalLightColorApportation(
                                        pixelPosWorld,
                                        pixelNormalWorld,
                                        diffColor.rgb,
                                        B_SampleRoughness(),
                                        B_LightForwardWorld,
                                        B_LightIntensity,
                                        B_LightColor.rgb,
                                        camPosWorld);
            dirLightApport *= lightness;

            B_GIn_Color = vec4(originalColor.rgb + dirLightApport, diffColor.a);
        }
        else { discard; }
    }
    else
    {
        // Should not arrive here, because pixels that do not receive light
        // are stenciled
        // TODO: This seems not to be being stenciled, fix this
        // B_GIn_Color = vec4(1,0,0,1);
        discard;
    }
}
