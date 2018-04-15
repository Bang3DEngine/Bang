#define BANG_FRAGMENT
#include "Common.glsl"

uniform vec3 B_PointLightPosition;

in vec4 B_FIn_Position;

out vec4 color;

void main()
{
    /*
    vec3 pointLightPosition = B_PointLightPosition;

    // Get distance between fragment and point light source
    float lightDistance = length(B_FIn_Position.xyz - pointLightPosition);
    lightDistance = lightDistance / B_Camera_ZFar; // Map to [0:1]
    gl_FragDepth = lightDistance; // Write this as depth
    gl_FragDepth = 0.7f;
    */
    // depth = 0.5f;
    // gl_FragColor = vec4(1,0,0,1);
    color = vec4(1,0,0,1);
}
