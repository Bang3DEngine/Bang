#define BANG_VERTEX
#include "Common.glsl"

layout(location = 0) in vec3 B_VIn_Position;
layout(location = 2) in vec2 B_VIn_Uv;

out vec2 B_FIn_AlbedoUv;

void main()
{
    vec4 v = B_PVM * vec4(B_VIn_Position, 1);
    vec2 uv = B_HasAlbedoTexture ? (B_VIn_Uv * B_AlbedoUvMultiply +
                                    B_AlbedoUvOffset) : B_VIn_Uv;
    B_FIn_AlbedoUv = uv;
    gl_Position    = v;
}
