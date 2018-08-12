#define BANG_VERTEX
#include "Common.glsl"

layout(location = 0) in vec3 B_VIn_Position;
layout(location = 1) in vec3 B_VIn_Normal;
layout(location = 2) in vec2 B_VIn_Uv;

out TGIn
{
    vec2 B_GIn_Uv;
}
B_VertexOut;

void main()
{
    B_VertexOut.B_GIn_Uv = B_VIn_Uv;
    gl_Position = (vec4(B_VIn_Position, 1.0));
}
