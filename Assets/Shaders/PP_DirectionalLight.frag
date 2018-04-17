#include "PP.frag"
#include "DirectionalLight.glsl"

void main()
{
    vec4 originalColor = B_SampleColor();
    vec3 pixelPosWorld = B_ComputeWorldPosition();
    vec3 pixelNormalWorld = B_SampleNormal();

    if (B_SampleReceivesLight())
    {
        float lightness = GetFragmentLightness(pixelPosWorld,
                                               pixelNormalWorld,
                                               B_LightForwardWorld);

        if (lightness > 0.0f)
        {
            vec4 diffColor = B_SampleDiffColor();
            vec3 dirLightApport = GetDirectionalLightColorApportation(
                                        pixelPosWorld,
                                        pixelNormalWorld,
                                        diffColor.rgb,
                                        B_SampleShininess(),
                                        B_LightForwardWorld,
                                        B_LightIntensity,
                                        B_LightColor.rgb,
                                        B_GetCameraPositionWorld() );
            dirLightApport *= lightness;

            B_GIn_Color = vec4(originalColor.rgb + dirLightApport,
                               diffColor.a);
        }
        else
        {
            B_GIn_Color = originalColor;
        }
    }
    else
    {
        // Should not arrive here, because pixels that do not receive light
        // are stenciled
        // TODO: This seems not to be being stenciled, fix this
        // B_GIn_Color = vec4(1,0,0,1);
        B_GIn_Color = originalColor;
    }
}
