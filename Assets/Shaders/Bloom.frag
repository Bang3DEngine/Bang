#include "ScreenPass.frag"

uniform sampler2D B_BlurredBloomTexture;
uniform bool B_ExtractingBrightPixels;

void main()
{
    vec2 uv = B_FIn_AlbedoUv;

    if (B_ExtractingBrightPixels)
    {
        vec4 inColor = B_SampleColor(uv);
        float brightness = Brightness(inColor.rgb);
        if (brightness >= 1)
        {
            B_GIn_Color = vec4(inColor.rgb, 1);
        }
        else
        {
            B_GIn_Color = vec4(0);
        }
    }
    else
    {
        B_GIn_Color = texture(B_BlurredBloomTexture, uv) * 10;
    }
}

