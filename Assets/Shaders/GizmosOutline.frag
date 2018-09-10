#include "ScreenPass.frag"

uniform vec4 B_OutlineColor;
uniform int B_OutlineThickness;
uniform float B_AlphaFadeOnDepth;
uniform sampler2D B_SceneDepthTexture;

float GetNearPixelDepth()
{
    int MinThickness = -B_OutlineThickness;
    int MaxThickness =  B_OutlineThickness;
    for (int i = MinThickness; i <= MaxThickness; ++i)
    {
        for (int j = MinThickness; j <= MaxThickness; ++j)
        {
            float neighborDepth = B_SampleDepthOffset( vec2(i,j) );
            if (neighborDepth < 1.0f)
            {
                return neighborDepth;
            }
        }
    }
    return 1.0f;
}

void main()
{
    float depth = B_SampleDepth();
    if (depth < 1.0f)
    {
        discard;
    }

    float nearPixelDepth = GetNearPixelDepth();
    if (nearPixelDepth < 1.0f)
    {
         vec2 uv = B_GetViewportUv();
         vec2 uvStep = B_GetViewportStep();
         vec2 uvPadding = (uvStep * B_OutlineThickness); // To avoid wrapping

         if (uv.x > uvPadding.x && uv.x < (1.0-uvPadding.x) &&
             uv.y > uvPadding.y && uv.y < (1.0-uvPadding.y))
         {
             float sceneDepth = texture(B_SceneDepthTexture, uv).r;
             float alpha = (nearPixelDepth > sceneDepth) ? B_AlphaFadeOnDepth : 1.0f;

             B_GIn_Color = B_OutlineColor * vec4(1, 1, 1, alpha);
             return;
         }
    }

    discard;
}
