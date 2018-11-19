#define BANG_FRAGMENT

#include "Common.glsl"

uniform sampler3D B_Texture3D;

in vec3 B_FIn_Color;

layout(location = 0) out vec4 B_GIn_Color;
layout(location = 1) out vec4 B_GIn_Albedo;
layout(location = 2) out vec4 B_GIn_Normal;
layout(location = 3) out vec4 B_GIn_Misc;

void main()
{
    vec4 color = vec4(B_FIn_Color, 1);
    color = vec4( vec3(texture3D(B_Texture3D, B_FIn_Color).r), 1 );

    B_GIn_Albedo = color;
    B_GIn_Normal = vec4(0);
    B_GIn_Misc   = vec4(0);
    B_GIn_Color  = color;
}
