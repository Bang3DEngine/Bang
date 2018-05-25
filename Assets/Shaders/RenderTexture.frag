#define BANG_FRAGMENT
#include "Common.glsl"

uniform float B_GammaCorrection;

layout(location = 0) out vec4 B_GIn_Color;

void main()
{
    vec2 uv = B_GetViewportUv() * B_AlbedoUvMultiply + B_AlbedoUvOffset;

    vec4 color = B_SampleColor(uv);
    float gammaCorrection = 1.0f/2.2f; // B_GammaCorrection;
    gl_FragColor = vec4(pow(color.rgb, vec3(gammaCorrection)), 1); // B_GammaCorrection));
    gl_FragDepth = B_SampleDepth(uv);
}
