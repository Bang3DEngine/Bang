#include "ScreenPass.frag"

uniform float B_FocusRange;
uniform float B_FocusDistanceWorld;
uniform float B_Fading;
uniform sampler2D B_SceneDepthTexture;
uniform sampler2D B_SceneColorTexture;
uniform sampler2D B_BlurredSceneColorTexture;

void main()
{
    vec2 uv = B_FIn_AlbedoUv;

    vec3 sceneColor = texture(B_SceneColorTexture, uv).rgb;
    vec3 blurredSceneColor = texture(B_BlurredSceneColorTexture, uv).rgb;

    float depth = texture(B_SceneDepthTexture, uv).r;
    float depthWorld = B_GetDepthWorld(depth);
    float distToFocus = distance(B_FocusDistanceWorld, depthWorld);
    // float distToFocusNorm = (distToFocus - B_FocusRange);
    float t = distToFocus;
    float FR = B_FocusRange;
    t = smoothstep((1 - B_Fading) * FR, (1 + B_Fading) * FR, t);
    t = (1.0f - t);

    B_GIn_Color = vec4(mix(blurredSceneColor, sceneColor, t), 1);
    // B_GIn_Color = vec4(vec3(t), 1);
    // B_GIn_Color = vec4(vec3(depthWorld), 1);
}

