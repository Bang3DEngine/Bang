#define BANG_FRAGMENT

#include "Common.glsl"

in vec3 B_FIn_Color;

layout(location = 0) out vec4 B_GIn_Color;
layout(location = 1) out vec4 B_GIn_Albedo;
layout(location = 2) out vec4 B_GIn_Normal;
layout(location = 3) out vec4 B_GIn_Misc;

void main()
{
    B_GIn_Albedo = vec4(B_FIn_Color.rgb, 1);
    B_GIn_Normal = vec4(0);
    B_GIn_Misc   = vec4(0);
    B_GIn_Color  = vec4(B_FIn_Color.rgb, 1);
}
