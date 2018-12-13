#define BANG_FRAGMENT
#define BANG_DEFERRED_RENDERING
#include "Common.glsl"
#include "LightCommon.glsl"

in vec4 B_FIn_Position;

void main()
{
    // Get distance between fragment and point light source
    float lightDistance = distance(B_FIn_Position.xyz, B_LightPositionWorld);
    lightDistance /= B_LightZFar; // Map to [0:1]
    gl_FragDepth = lightDistance; // Write this as depth
}
