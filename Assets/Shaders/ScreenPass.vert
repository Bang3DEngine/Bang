#define BANG_VERTEX
#include "Common.glsl"

layout(location = 0) in vec3 B_VIn_Position;
layout(location = 2) in vec2 B_VIn_Uv;

out vec2 B_FIn_AlbedoUv;

void main()
{
    B_FIn_AlbedoUv = B_VIn_Uv;
    gl_Position = vec4(B_VIn_Position, 1);
}
