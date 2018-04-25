#define BANG_VERTEX
#include "Common.glsl"

layout(location = 0) in vec3 B_VIn_Position;
layout(location = 1) in vec3 B_VIn_Normal;
layout(location = 2) in vec2 B_VIn_Uv;
layout(location = 3) in vec3 B_VIn_Tangent;

out vec2 B_FIn_AlbedoUv;

void main()
{
    vec4 v = B_PVM * vec4(B_VIn_Position, 1);
    B_FIn_AlbedoUv = B_VIn_Uv;
    gl_Position    = v;
}
