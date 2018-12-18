#include "ScreenPass.frag"

uniform sampler2D B_SSAOMap;
uniform float B_SSAOIntensity;

void main()
{
    vec2 uv = B_FIn_AlbedoUv;
    vec4 inColor = B_SampleColor();
    float ssao = texture(B_SSAOMap, uv).r;
    ssao = ((1.0f - ssao * B_SSAOIntensity));
    B_GIn_Color = vec4(inColor.rgb * ssao, inColor.a);
}
