#define BANG_VERTEX
#define BANG_DEFERRED_RENDERING

#include "Common.glsl"

layout(location = 0) in vec3 B_VIn_Position;
layout(location = 1) in vec3 B_VIn_Normal;
layout(location = 2) in vec2 B_VIn_Uv;
layout(location = 3) in vec3 B_VIn_Tangent;

out vec3 B_FIn_ModelPosition;

void main()
{
    vec2 uv = vec2(B_VIn_Uv.x, 1.0 - B_VIn_Uv.y);

    vec4 modelPosition = vec4(B_VIn_Position, 1);
    vec4 modelNormal   = vec4(B_VIn_Normal,   0);

    B_FIn_ModelPosition = modelPosition.xyz;

    gl_Position = B_PVM * modelPosition;
}
