#include "ScreenPass.frag"

uniform vec4 B_OutlineColor;
uniform int B_OutlineThickness;
uniform float B_AlphaFadeOnDepth;
uniform sampler2D B_OutlineDepthTexture;
uniform sampler2D B_SceneDepthTexture;


float GetNearPixelDepth(vec2 uv, vec2 uvStep)
{
    int MinThickness = -B_OutlineThickness;
    int MaxThickness =  B_OutlineThickness;
    for (int i = MinThickness; i <= MaxThickness; ++i)
    {
        for (int j = MinThickness; j <= MaxThickness; ++j)
        {
            float neighborOutlineObjDepth = texture(B_OutlineDepthTexture,
                                                    uv + uvStep * vec2(i,j) ).r;
            if (neighborOutlineObjDepth < 1.0f)
            {
                return neighborOutlineObjDepth;
            }
        }
    }
    return 1.0f;
}

void main()
{
    vec2 uv = B_GetViewportUv();
    vec2 uvStep = B_GetViewportStep();

    float outlineObjDepth = texture(B_OutlineDepthTexture, uv).r;
    if (outlineObjDepth >= 1.0f)
    {
        float nearPixelDepth = GetNearPixelDepth(uv, uvStep);
        if (nearPixelDepth < 1.0f)
        {
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
    }
    discard;
}
