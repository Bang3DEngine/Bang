#include "ScreenPass.frag"

uniform float B_Intensity;
uniform float B_BrightnessThreshold;

uniform sampler2D B_SceneColorTexture;
uniform sampler2D B_SceneLightTexture;
uniform sampler2D B_BlurredBloomTexture;
uniform bool B_ExtractingBrightPixels;

void main()
{
    vec2 uv = B_FIn_AlbedoUv;

    if (B_ExtractingBrightPixels)
    {
        vec4 inLight = texture(B_SceneLightTexture, uv);
        float brightness = Brightness(inLight.rgb);
        if (brightness >= B_BrightnessThreshold)
        {
            B_GIn_Color = vec4(inLight.rgb, 1);
        }
        else
        {
            B_GIn_Color = vec4(0, 0, 0, 1);
        }
    }
    else
    {
        vec4 inColor = texture(B_SceneColorTexture, uv);
        vec3 bloom = texture(B_BlurredBloomTexture, uv).rgb;
        B_GIn_Color = vec4(inColor.rgb + (bloom.rgb * B_Intensity), 1);
    }
}

