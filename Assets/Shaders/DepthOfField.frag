#include "ScreenPass.frag"

uniform float B_NearDistance;
uniform float B_NearFadingSize;
uniform float B_NearFadingSlope;
uniform float B_FarDistance;
uniform float B_FarFadingSize;
uniform float B_FarFadingSlope;
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

    float t = 0.0f;
    // t = smoothstep((1 - B_Fading) * FR, (1 + B_Fading) * FR, t);
    t += (1 - smoothstep(B_NearDistance + B_NearFadingSize * (1-B_NearFadingSlope),
                         B_NearDistance + B_NearFadingSize,
                         depthWorld));

    t += (smoothstep(B_FarDistance + B_FarFadingSize * (1-B_FarFadingSlope),
                     B_FarDistance + B_FarFadingSize,
                     depthWorld));
    t = clamp(t, 0, 1);

    B_GIn_Color = vec4(mix(sceneColor, blurredSceneColor, t), 1);
    // B_GIn_Color = vec4(vec3(t), 1);
    // B_GIn_Color = vec4(vec3(depthWorld), 1);
}

