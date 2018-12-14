#define BANG_FRAGMENT
#define BANG_DEFERRED_RENDERING

#include "Common.glsl"
#include "LightCommon.glsl"

in vec4 B_FIn_Position;

layout(location = 0) out vec4 B_OutColor;

void main()
{
    // Get distance between fragment and point light source
    float lightDistance = 1.0f / gl_FragCoord.w; // gl_FragCoord.z / gl_FragCoord.w;
    // float lightDistance = distance(B_FIn_Position.xyz, B_LightPositionWorld);
    float depth = lightDistance; // Map01(lightDistance, B_LightZNear, B_LightZFar) * 2.0f - 1.0f;

    // vec3 rayDir = normalize(B_FIn_Position.xyz - B_LightPositionWorld);

    float expDepth = exp(B_LightShadowExponentConstant * depth);
    B_OutColor = vec4(vec3(expDepth), 1);

    // B_OutColor = vec4(rayDir, 1);
}
