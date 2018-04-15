#define BANG_FRAGMENT
#include "Common.glsl"
#include "LightCommon.glsl"

uniform float B_PointLightZFar;

in vec4 B_FIn_Position;

void main()
{
    // Get distance between fragment and point light source
    float lightDistance = distance(B_FIn_Position.xyz, B_LightPositionWorld);
    lightDistance /= B_PointLightZFar; // Map to [0:1]
    gl_FragDepth = lightDistance; // Write this as depth
}
