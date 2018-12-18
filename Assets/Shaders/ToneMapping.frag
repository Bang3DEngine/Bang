#include "ScreenPass.frag"

uniform float B_Exposure;

void main()
{
    vec3 inHDRColor = B_SampleColor().rgb;

    float exposure = B_Exposure;
    vec3 outColor = vec3(1.0) - exp(-inHDRColor * exposure);

    B_GIn_Color = vec4(outColor, 1.0);
}

