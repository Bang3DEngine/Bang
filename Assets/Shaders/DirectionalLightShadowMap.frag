#define BANG_FRAGMENT
#define BANG_DEFERRED_RENDERING
#define BANG_USE_REFLECTION_PROBES
#include "Common.glsl"
#include "LightCommon.glsl"

layout(location = 0) out vec4 B_OutDepth;

void main()
{
    float depth = gl_FragCoord.z;
    float expDepth = exp(B_LightShadowExponentConstant * depth);
    B_OutDepth = vec4(expDepth, 0, 0, 1);
}
