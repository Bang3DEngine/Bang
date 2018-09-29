#define BANG_FRAGMENT
#define BANG_DEFERRED_RENDERING

#define BANG_SUPPRESS_DEFAULT_FRAG_COMMON_MAIN
#include "DefaultFragCommon.glsl"

in vec4 B_FIn_ParticleColor;

void main()
{
    DefaultFragCommonMain(B_FIn_ParticleColor);
}
