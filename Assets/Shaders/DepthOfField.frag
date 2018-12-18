#include "ScreenPass.frag"

uniform float B_FadeRange;
uniform float B_ForegroundThreshold;
uniform sampler2D B_SceneDepthTexture;
uniform sampler2D B_SceneBackgroundColorTexture;
uniform sampler2D B_SceneForegroundColorTexture;

void main()
{
    vec2 uv = B_FIn_AlbedoUv;

    vec3 sceneBGColor = texture(B_SceneBackgroundColorTexture, uv).rgb;
    vec3 sceneFGColor = texture(B_SceneForegroundColorTexture, uv).rgb;

    float depth = texture(B_SceneDepthTexture, uv).r;
    float fgThreshold = (B_ForegroundThreshold);
    float t = (1.0f - smoothstep(fgThreshold * (1.0f - B_FadeRange),
                                 fgThreshold * (1.0f + B_FadeRange),
                                 depth));

    B_GIn_Color = vec4(mix(sceneBGColor, sceneFGColor, t), 1);
}

