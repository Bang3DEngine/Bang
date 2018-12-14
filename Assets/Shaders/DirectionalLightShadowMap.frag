#define BANG_FRAGMENT
#define BANG_DEFERRED_RENDERING
#include "Common.glsl"
#include "LightCommon.glsl"

layout(location = 0) out vec4 B_OutDepth;

void main()
{
    float depth = gl_FragCoord.z * 2.0f - 1.0f;
    float expDepth = exp(B_LightShadowExponentConstant * depth);
    B_OutDepth = vec4(expDepth, 0, 0, 1); // Write this as depth
}
