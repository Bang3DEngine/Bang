#include "ScreenPass.frag"

uniform sampler2D B_SSAOMap;

void main()
{
    vec4 inColor = B_SampleColor();
    float ssao = texture(B_SSAOMap, B_GetViewportUv()).x;
    B_GIn_Color = vec4( (inColor.rgb * (vec3(1.0) - vec3(ssao))), inColor.a);
}
