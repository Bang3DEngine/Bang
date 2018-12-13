#define BANG_FRAGMENT
#define BANG_DEFERRED_RENDERING
#include "Common.glsl"
#include "LightCommon.glsl"

layout(location = 0) out vec4 B_OutDepth;

void main()
{
    // Get distance between fragment and point light source
    float lightDistance = gl_FragCoord.z * 2.0f - 1.0f; // distance(B_FIn_Position.xyz, B_Light);
    // lightDistance = Map01(lightDistance, B_LightZNear, B_LightZFar); // Map to [0:1]

    float expDepth = exp(LIGHT_EXP * lightDistance);
    B_OutDepth = vec4(expDepth, expDepth, expDepth, 1); // Write this as depth
}
