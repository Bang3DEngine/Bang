#define BANG_FRAGMENT
#include "Common.glsl"

in vec3 B_FIn_Position;
in vec3 B_FIn_Normal;
in vec2 B_FIn_AlbedoUv;

layout(location = 0) out vec4 B_GIn_Color;
layout(location = 1) out vec4 B_GIn_Albedo;
layout(location = 2) out vec4 B_GIn_Normal;
layout(location = 3) out vec4 B_GIn_Misc;

void main()
{
    B_GIn_Albedo  = B_MaterialAlbedoColor * texColor;
    B_GIn_Normal   = vec4(0, 0, 0, 1);
    B_GIn_Misc.x   = 0;
    B_GIn_Color = vec4(1, 0, 1, 1);
}
