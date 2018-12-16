#include "ScreenPass.frag"

uniform float B_Intensity;
uniform float B_BrightnessThreshold;

uniform sampler2D B_SceneTexture;
uniform sampler2D B_BlurredBloomTexture;
uniform bool B_ExtractingBrightPixels;

void main()
{
    vec2 uv = B_FIn_AlbedoUv;

    vec4 inColor = texture(B_SceneTexture, uv);
    if (B_ExtractingBrightPixels)
    {
        float brightness = Brightness(inColor.rgb);
        if (brightness >= B_BrightnessThreshold)
        {
            B_GIn_Color = vec4(inColor.rgb, 1);
        }
        else
        {
            B_GIn_Color = vec4(0, 0, 0, 1);
        }
    }
    else
    {
        vec3 bloom = texture(B_BlurredBloomTexture, uv).rgb;
        B_GIn_Color = vec4(inColor.rgb + (bloom.rgb * B_Intensity), 1);
    }
}

